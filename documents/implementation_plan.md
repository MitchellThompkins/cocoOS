# cocoOS Fork — Completion Plan (`monolithic-fix` branch)

**Last updated:** 2026-07-06 (supersedes `documents/work_remaining.md`)
**Branch:** `monolithic-fix` (based on `develop`), PR #13 open
**Publish target when done:** merge `monolithic-fix` → `develop` (PR #13), then `develop` → `master`

---

## 0. Status — what is already DONE (verified 2026-07-06)

Phases 0–6 of the original plan are complete, in commits `aa64d4c..106780b`.
`documents/state.md` records exactly what changed. Verified today with a full
clean run from the host:

```sh
make ci        # clean build a9 + x86_64, all 6 suites pass on host + QEMU,
               # check-trace: "All requirements are traced" (all 70 rows)
```

Done and verified — do not redo:

- **OS bug fixes:** `OS_SIGNAL_SEM` tid regression, `os_task_wait_sem_set`
  rename, `msg_receive_async` arity, `os_msgQ_tick` change-event signaling,
  msgqueue division-by-zero guards, `os_task_create` semaphore init.
- **Test infra:** `checkExpectations()` in all teardowns, `trace_reqs.py` exits
  nonzero on missing/duplicate traces, msgqueue + event mocks completed/fixed.
- **All 70 requirements traced**; the 29 previously-missing tests written.
- **Style:** compile flags actually applied (`-Wall`, `-Werror=return-type` on
  os/), CppUTest pinned, README rewritten, docker-compose/makefile polish,
  `MsgQResult_t` enum named, doxygen stubs filled.
- **Automation:** `make ci` and `make container.run CMD=...` targets work
  non-interactively from the host.
- **Integration smoke test** `test_integration` (public_api_smoke_test) links
  all real impls, runs on both platforms, wired into presets + test runner.
- **CI workflow:** checkout action pinned, weekly cron added.

Remaining work is Phases 8–11 below (Phase 7 = publish, now last). The
overriding directive for this round, from Mitchell:

> **Every core functionality and key function needs an explicit test.**

"Explicit" means: the component's own test suite calls the function directly
and asserts its behavior — indirect exercise through another function or a
macro expansion does not count.

---

## 1. Context (unchanged — read once)

Fork of [cocoOS](https://github.com/cocoOS/cocoOS) split into independently
testable components (`os/os_kernel`, `os_task`, `os_event`, `os_sem`,
`os_msgqueue`, `os_utils`), each an `_intf`/`_impl`/`_impl_mock` CMake library.
Behavioral requirements live in `documents/requirements.csv`; every test tags
the requirements it verifies with `UT_CATALOG_ID("REQ-ID")`; junit XML
`<system-out>` is checked by `scripts/trace_reqs.py` (`make check-trace`,
fails on untraced/duplicate). Tests run on host `x86_64` and `cortex-a9`
under QEMU, inside the `ghcr.io/mitchellthompkins/embedded_sdk` container.

Build/test loop (all from host):

```sh
make ci                                  # full gate: build both + test + trace
make container.run CMD='cmake --build --preset x86_64 && ./build/x86_64/test_os_task.elf'
```

Run `make ci` as the acceptance gate after every phase.

---

## 2. Phase 8 — Explicit function-level test coverage  ← START HERE

### 8.1 Audit results (verified 2026-07-06, via `nm` on impl libs + grep of test sources)

Public functions (defined `T` symbols in `libos_*_impl.a`) vs direct calls in
the component's own test file:

- `os_event`, `os_sem`, `os_msgqueue`, `os_kernel`: **every public function is
  directly called** in its suite. No gaps.
- `os_task`: the following have **no direct call in any test file**:

| Function | Used by (in-tree) | Action |
|---|---|---|
| `os_task_set_wait_queue` / `os_task_get_wait_queue` | `OS_MSG_Q_POST/RECEIVE` macros | explicit test |
| `os_task_set_change_event` / `os_task_get_change_event` | `OS_MSG_Q_POST/RECEIVE` macros | explicit test |
| `os_task_set_msg_result` / `os_task_get_msg_result` | `OS_MSG_Q_POST/RECEIVE` macros | explicit test |
| `os_task_ready_set` | os_task internals + public header | explicit test |
| `os_task_internal_state_set` | `OS_YIELD` macro | explicit test (pair with `task_internal_state_get`, which is tested) |
| `task_run` | `os_schedule()` in os_kernel.c | explicit test |
| `os_task_run_test` | **nobody — declared in os_task.h:112, never defined** | delete the declaration |
| `task_should_run_test` | **nobody** (defined os_task.c, self-referential magic `state==99`) | delete, or justify+test — recommend delete |
| `task_set_no_running_task` | **nobody** (defined os_task.c:765, not even declared in the header) | delete |

- `os_utils`: **has no test suite at all.** `os_on_assert` and
  `os_on_assert_attach_callback` (os_assert.c) are core — every assert path in
  the OS depends on them — and are only ever replaced by a mock in the other
  suites. Needs a real suite (§8.4).

### 8.2 New tests in `tests/os_task/src/os_task.test.cpp`

For each function below: new TEST, tagged with a **new requirement row**
(§8.5). Follow existing file style.

1. `os_task_set_wait_queue(tid, q)` then `os_task_get_wait_queue(tid) == q`;
   distinct values on two tasks don't interfere.
2. `os_task_set_change_event(tid, e)` / `os_task_get_change_event(tid)` — same
   pattern.
3. `os_task_set_msg_result(tid, r)` / `os_task_get_msg_result(tid)` — same
   pattern.
4. `os_task_ready_set(tid)`: put a task in a non-READY state (e.g.
   `os_task_suspend`), call it, `task_state_get(tid) == READY`.
5. `os_task_internal_state_set(tid, s)` then `task_internal_state_get(tid) == s`.
   Include the `OS_YIELD` re-entry semantics sanity: value round-trips exactly.
6. `task_run()`: set `running_tid` via the kernel mock
   (`mock().setData("running_tid_from_get", tid)`), give the task a proc that
   sets a flag; call `task_run()`; assert the proc ran and the task's state
   transitioned as expected. (Check how `task_run` reads the running tid — it
   is in the REAL os_task lib but the kernel is mocked in this suite, so
   `os_get_running_tid` comes from `mock_os_kernel.c`.)

### 8.3 Dead-code removal (part of this phase, keeps the audit clean)

- Delete `os_task_run_test` declaration (`os/os_task/include/os_task.h:112`) —
  there is no definition anywhere; any caller would fail to link.
- Delete `task_should_run_test` (os_task.h + os_task.c + any mock entry) unless
  a caller is discovered; the `state==0 || state==99` magic is orphaned test
  scaffolding.
- Delete `task_set_no_running_task` (os_task.c:765) — no declaration, no
  callers.
- After removal, rerun the audit (§8.6) — the os_task list must be 100% OK.

### 8.4 New suite: `tests/os_utils/`

New test target `test_os_utils` mirroring the other suites' CMake structure
(links REAL `os_utils_impl`; no other os libs needed — check what `os_port.h`
`os_disable_interrupts` needs from `platform`). Add the target to both preset
target lists in `CMakePresets.json` and its junit XML to the `check-trace`
list in the makefile.

**Blocker to solve first:** `os_on_assert` ends in `while(1);` even when a
user callback is attached (`os/os_utils/src/os_assert.c`). A unit test calling
it would hang. Fix the production code, not the test:

- If a user callback is attached, **return** after invoking it (document the
  contract: attaching a callback converts the assert from halt to notify —
  it's explicitly the test/development hook; with no callback attached the
  behavior stays `while(1)`).
- This also fixes a real defect in the integration test: its assert guard sets
  a flag and returns, after which `while(1)` **hangs the QEMU CI job forever**
  instead of failing the test (`tests/integration/src/integration.test.cpp:33-38`
  + teardown CHECK). After the fix, an assert during integration produces a
  clean test failure.

Tests (tag with new UTILS-x requirements, §8.5):

1. `os_on_assert_attach_callback(cb)` + `os_on_assert("f.c", 42, "x>0")` →
   cb invoked once with exactly those arguments; function returns.
2. Callback replacement: attach cb2, assert again → only cb2 fires.
3. `os_assert(false)` macro inside a helper void function → triggers
   `os_on_assert` with `__FILE__`/`__LINE__`/stringized expression and returns
   out of the helper; `os_assert(true)` → callback NOT invoked.
4. `os_assert_with_return(false, 7)` in a helper returning int → returns 7 and
   fires the callback.

Also fix while here: the `#else` (NASSERT) branch of `os_assert.h` defines only
`os_assert`, not `os_assert_with_return` — any file using the latter fails to
compile with NASSERT. Add the empty variant. (A compile-only check is enough;
no runtime test needed — optionally add a tiny `#define NASSERT` TU to the
os_utils test target to prove it compiles.)

### 8.5 New requirement rows (`documents/requirements.csv`)

The traceability model requires every test to tag a requirement. Add rows —
follow the tightened "shall" style of the KERNEL/MSGQUEUE rows:

- `TASK-26` — wait-queue accessor pair (set/get per task).
- `TASK-27` — change-event accessor pair.
- `TASK-28` — msg-result accessor pair.
- `TASK-29` — `os_task_ready_set` puts the specified task into READY.
- `TASK-30` — `os_task_internal_state_set` stores the coroutine re-entry state
  retrievable via `task_internal_state_get`.
- `TASK-31` — `task_run` executes the currently scheduled task's procedure.
- `UTILS-1` — assert macro contract (triggers `os_on_assert` with
  file/line/expression when the condition is false; no-op when true).
- `UTILS-2` — `os_on_assert_attach_callback` registers a callback invoked on
  assert; attaching replaces the previous callback; when a callback is
  attached `os_on_assert` returns instead of halting.
- `UTILS-3` — `os_assert_with_return` returns the provided value on failure.

Keep IDs stable; never renumber existing rows (tests reference them).

### 8.6 Make the function-coverage audit permanent (recommended)

Add `scripts/check_function_coverage.py`: for each component, list defined `T`
symbols from `build/x86_64/os/<lib>/lib<lib>_impl.a` (`nm -g --defined-only`),
grep `tests/<lib>/src/*.cpp` for a direct call of each, fail on misses, with a
small allowlist (e.g. `os_cbkSleep` is covered in the kernel suite via weak
override). Wire as `make check-coverage`, add to `make ci` and the CI workflow.
This encodes Mitchell's "every key function has an explicit test" rule so it
can't rot. (~40 lines of python; mirror the style of `trace_reqs.py`.)

---

## 3. Phase 9 — Apply the requirements-quality findings

`documents/req-findings.md` (written during the last session) documents CSV
defects but none are applied yet. Apply them, then delete `req-findings.md`:

1. **Column header** `test_case_id` → `requirement_id`. Must be changed in
   lockstep with `scripts/trace_reqs.py` (`row['test_case_id']`) — grep for
   other consumers before renaming.
2. **Normalize style**: every row a "shall" sentence; product name spelled
   `cocoOS` everywhere (currently also CocoOS/CocoOs).
3. **TASK-1**: replace "bad data" with the three concrete triggers (null task
   procedure, duplicate priority, exceeding N_TASKS) — either enumerated in one
   row or split into TASK-1a-style separate new rows (new IDs, don't renumber).
4. **TASK-2**: state the observable consequence (os_assert triggers).
5. **TASK-17**: describes file-static `task_killed_set`; observable behavior
   already covered by TASK-7. Delete the row AND remove its `UT_CATALOG_ID`
   tag from the test (gaps in numbering are fine; `trace_reqs.py` doesn't
   require contiguity — verify after).
6. **TASK-19**: says "clear the event wait queue of **all tasks**" but
   `os_task_clear_wait_queue(tid)` takes a single tid. Fix the text to match
   the code (per-task) after confirming against `os_task.c`.
7. **TASK-22**: name the function (`task_tick`).
8. **KERNEL-1 / KERNEL-2**: repair the broken grammar ("stops and the
   re-starts", "tasks that do not execute only once") while preserving intent.
9. **MSGQUEUE-10 vs MSGQUEUE-11 overlap**: keep both but differentiate —
   10 = returns EMPTY when *all* pending messages are delayed; 11 = a ready
   message is delivered even when *older* messages in the queue are still
   delayed (ordering/skip behavior). Make the tests match the sharpened text.
10. **EVENT-5**: inline the referenced behaviors instead of "invokes the
    behaviors described by `os_task_wait_event`".

Acceptance: `make ci` green (trace still complete after tag/row changes).

---

## 4. Phase 10 — Close the macro-coverage gaps in the smoke test

Verified today: these public macros are still never expanded by any test —
the exact latent-bug class the smoke test exists to catch:

`task_wait_id`, `event_wait_timeout` (+`_ex`), `event_get_timeout`,
`event_wait_multiple`, `event_wait_ex`, `event_ISR_signal`, `sem_ISR_signal`,
`msg_receive_ex`.

Extend `tests/integration/src/integration.test.cpp` (new TEST cases in the
group are fine — each does its own `os_init()` + task setup in `setup()`):

- **Sub-clock test**: a task using `task_wait_id(2, n)`; drive with
  `os_sub_tick(2)` / `os_sub_nTick(2, k)` interleaved with `unit_test_os_schedule()`;
  assert master-clock ticks do NOT advance it.
- **Event timeout test**: one task `event_wait_timeout(evt, 5)` where the event
  is never signaled → resumes after 5 ticks and `event_get_timeout() == 0`;
  another waits with timeout but IS signaled early → `event_get_timeout() != 0`.
- **Multi-event test**: `event_wait_multiple(0, e1, e2)` (any) and
  `event_wait_multiple(1, e1, e2)` (all); assert wake conditions and
  `event_last_signaled_get()`.
- **ISR variants**: from the test driver (outside any task), call
  `sem_ISR_signal(sem)` and `event_ISR_signal(evt)`; assert the blocked task
  becomes READY on the next schedule pass and
  `event_signaling_taskId_get(evt) == ISR_TID` where applicable.
- **Callback variants**: `msg_receive_ex` / `event_wait_ex` with a callback
  that sets a flag — assert the callback fired before the block.

Notes:
- The Phase 8.4 assert fix must land first so a hang becomes a failure.
- Tag new tests honestly with existing requirement IDs they exercise
  (KERNEL-6, KERNEL-15, EVENT-5, EVENT-6, TASK-21…). If a macro's behavior has
  no covering requirement, add a row (same rules as §8.5).
- Add `cpputest_Integration.xml` to the `check-trace` XML list in the makefile
  — it is currently generated but not read, so integration `UT_CATALOG_ID`
  tags are invisible to the trace check.

---

## 5. Phase 11 — Housekeeping

- Commit `documents/implementation_plan.md` and `documents/state.md` (still
  untracked). Delete `documents/req-findings.md` once Phase 9 lands.
- Delete the stray local branch `list` (`git branch -D list`).
- Keep PR #13 description in sync with the final scope (it exists already:
  https://github.com/MitchellThompkins/cocoOS/pull/13).

---

## 6. Phase 7 (last) — Final verification & publish

1. `make ci` from the host — build both platforms, all suites (now 7 targets),
   `check-trace` green, `check-coverage` green (if §8.6 adopted).
2. `git clean -ndx` sanity: nothing unexpected tracked; `cpputest_*.xml`,
   `build/` stay ignored.
3. Phase-sized commits with clear messages.
4. CI workflow already runs `make build.all` + `make test check-trace`; add
   `check-coverage` if adopted.
5. Merge PR #13 → `develop`. Then `develop` → `master`. **Ask Mitchell before
   pushing to `master`.**

---

## 7. Acceptance checklist

- [ ] `make ci` single-command green: a9 + x86_64 build, all suites on host +
      QEMU, trace check enforcing and complete.
- [x] All requirements traced (70 rows — verified 2026-07-06; count will grow
      with §8.5/§10 additions).
- [ ] **Every public function of every component has a direct, explicit test
      in its component suite** (os_task accessor/scheduling gaps closed,
      os_utils suite added); dead functions removed instead of tested.
- [ ] `check_function_coverage.py` (or documented equivalent) prevents
      regression of the above.
- [ ] `os_on_assert` returns when a callback is attached; integration assert
      guard fails instead of hanging; NASSERT config compiles.
- [ ] Every user-facing macro expanded at least once by the smoke test,
      including `task_wait_id`, `event_wait_timeout`, `event_get_timeout`,
      `event_wait_multiple`, ISR signal variants, `_ex` callback variants.
- [ ] `cpputest_Integration.xml` included in check-trace.
- [ ] req-findings.md items applied to requirements.csv and deleted.
- [ ] PR #13 merged to `develop`; `develop` → `master` sync (with Mitchell's
      sign-off).

---

## Appendix — audit method (for §8.6 and future re-runs)

```sh
# inside or outside the container; needs a built x86_64 tree
for lib in os_task os_event os_sem os_msgqueue os_kernel; do
  nm -g --defined-only build/x86_64/os/$lib/lib${lib}_impl.a \
    | grep " T " | awk '{print $NF}' | sort -u | while read fn; do
      grep -qE "(^|[^a-zA-Z0-9_])${fn} *\(" tests/$lib/src/$lib.test.cpp \
        && echo "OK   $fn" || echo "MISS $fn"
  done
done
```
