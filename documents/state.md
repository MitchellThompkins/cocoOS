# Branch State

Branch: `monolithic-fix`
Base: `develop`
PR: https://github.com/MitchellThompkins/cocoOS/pull/13

All commits on this branch ahead of develop, in order:

```
8819001 quick draft
1593a39 use modern docker compose
6cf5a17 container start fix
aa64d4c fix real OS bugs and add complete unit test suite (Phases 0-4)
b41be89 style/hygiene fixes (Phase 5)
d5f7936 add integration smoke test
8b64191 pin actions/checkout to v6.0.3 commit hash
ad8a255 rename integration test to public_api_smoke_test
b6f9b4c run CI on every push and weekly (Sunday midnight UTC)
106780b remove non-ASCII characters from comments and documentation
```

---

## OS Bug Fixes

### os/os_sem/include/os_sem.h

`OS_SIGNAL_SEM` and `OS_SIGNAL_SEM_NO_SCHEDULE` compared the result of `os_task_waiting_this_semaphore()` to `== 0`. That function was changed on this branch to return the waiting tid as an `int16_t` (or `-1` for none), so `== 0` incorrectly matched task 0 as the "nobody waiting" case. Changed to `< 0`.

`OS_WAIT_SEM` called `os_task_wait_sem_set(...)` but the function was named `task_wait_sem_set` everywhere else. Renamed to `os_task_wait_sem_set` throughout (os_task.h, os_task.c, mock_os_task.c, os_task.test.cpp).

### os/os_kernel/include/os_kernel.h

`msg_receive_async(task_id, pMsg)` expanded to `OS_MSG_Q_RECEIVE(task_id, pMsg, 1)` but the macro takes four arguments. Fixed to `OS_MSG_Q_RECEIVE(task_id, pMsg, 1, 0)`.

### os/os_msgqueue/src/os_msgqueue.c

`os_msgQ_tick` had a commented-out `event_ISR_signal` call with a `TODO` note. Replaced with direct calls to `os_signal_event` and `os_event_set_signaling_tid` so that a delayed message reaching zero ticks signals the queue change event and unblocks a waiting receiver.

Added zero-size guards in `os_msg_receive` and `os_msgQ_tick` to prevent division by zero when a queue is created with size zero.

### os/os_task/src/os_task.c

`os_task_create` set `task->semaphore = 0` (a valid semaphore id). Changed to `NO_SEM` to match `os_task_init`.

---

## Test Infrastructure Fixes

### All five test suite teardowns

Changed from `mock().clear()` to `mock().checkExpectations(); mock().clear()`. Without `checkExpectations`, stale `expectOneCall` entries silently pass.

### scripts/trace_reqs.py

Was always returning 0. Changed to return 1 when `missing_traces` or `duplicates` is non-empty, so `make check-trace` and CI can actually fail on regressions. Also added detection of duplicate requirement IDs in the CSV.

### os/os_msgqueue/mocks/src/mock_os_msgqueue.c

Was a stub containing only `os_msgQ_init`. Test binaries that linked it were silently using real msgqueue implementations for all other functions. Added complete mocks for `os_msgQ_create`, `os_msgQ_find`, `os_msgQ_event_get`, `os_msgQ_tick`, `os_msg_post`, `os_msg_receive`.

### os/os_event/mocks/src/mock_os_event.c

`event_create` and `event_last_signaled_get` fell off the end of non-void functions. Fixed to return mock values.

---

## 29 Missing Tests Written

All previously untraced requirements now have tests. Files changed:

**tests/os_task/src/os_task.test.cpp** added tests for TASK-4, TASK-5, TASK-6, TASK-7, TASK-8, TASK-10, TASK-15, TASK-16, TASK-17, TASK-19, TASK-20, TASK-24, TASK-25.

**tests/os_kernel/src/os_kernel.test.cpp** added tests for KERNEL-7, KERNEL-8, KERNEL-9, KERNEL-10, KERNEL-11 (traced from round-robin test), KERNEL-12, KERNEL-13, KERNEL-14, KERNEL-15. Also fixed UB: `int*` cast of a `uint16_t` data pointer changed to `uint8_t*`; loop counter changed from `int` to `size_t`; unused `const auto id` variables changed to `(void)os_task_create(...)`; `uint32_t` casts added to CHECK_EQUAL comparisons against `os_task_timeout_get` return.

**tests/os_msgqueue/src/os_msgqueue.test.cpp** added tests for MSGQUEUE-1, MSGQUEUE-3, MSGQUEUE-5, MSGQUEUE-6, MSGQUEUE-13, MSGQUEUE-14, MSGQUEUE-15.

All 70 requirements in `documents/requirements.csv` now have a `UT_CATALOG_ID` trace in at least one test.

---

## Style and Hygiene

### tests/CMakeLists.txt

`add_compile_options(-std=c++14 -Wall)` was placed after the `add_subdirectory` calls, so it applied to nothing. Moved before all subdirectory adds. Changed to `$<$<COMPILE_LANGUAGE:CXX>:-std=c++14>` so the C++ standard flag does not get passed to C compilation units.

CppUTest was fetched at `GIT_TAG master` (non-reproducible). Pinned to commit `79c066ef`. The v4.0 release tag was not used because it lacks the `CppUTest::CppUTest` CMake alias that the build requires.

Added `add_subdirectory(integration)`.

### os/CMakeLists.txt

Added `add_compile_options(-Wall -Werror=return-type)` before the subdirectory adds so the OS library sources build with warnings enabled.

### os/os_task/mocks/src/mock_os_task.c

`os_task_prio_get` had no return statement, causing UB. Added proper mock return.

### os/os_sem/mocks/src/mock_os_sem.c

Removed unused `SemValue_t` typedef, `semList[]` static array, and `nSemaphores` static that were generating warnings.

### os/os_msgqueue/include/os_msgqueue.h

Named the anonymous result enum `MsgQResult_t`. Updated `os_msg_post` and `os_msg_receive` return types. Updated the `OS_MSG_Q_POST` and `OS_MSG_Q_RECEIVE` macro locals accordingly. Filled in empty doxygen `@brief` stubs. Removed the commented-out `os_msgQ_sem_get` block.

### os/os_event/include/os_event.h

Replaced `// TODO` doxygen briefs on `os_signal_event` and `os_event_set_signaling_tid` with real descriptions.

### tests/platforms/arm/cortex-a9/src/timer.c

`init_gtimer()` was declared to return `int` but fell off the end. Added `return NO_ERR`.

### tests/platforms/x86_64/src/platform.c

`tick()` was declared `static void*` but fell off the end. Added `return NULL`.

### docker-compose.yml

Removed the obsolete `version: '3'` line. Fixed `Display=` environment variable to `DISPLAY=`.

### makefile

Guarded `pip install termcolor` in both the `test` and `check-trace` targets with a Python import check to avoid re-running the install on every invocation.

### README.md

Rewrote the Configuration section to document that all six macros are required (not optional), that they live in a user-provided `user_os_config.h`, and that `#error` fires if any are missing.

Updated `os_start(0)` to document the `tick_limit` parameter behavior.

Updated `os_task_create` parameter documentation.

Added a "Building and Running the Tests" section covering the Docker image requirement, `make ci`, `make container.run`, and the `UT_CATALOG_ID` traceability workflow.

### documents/work_remaining.md

Deleted. Superseded by `documents/implementation_plan.md`.

---

## Integration Test

New target `test_integration` under `tests/integration/`.

Links all real OS implementations with no mocks: `os_kernel_impl`, `os_task_impl`, `os_event_impl`, `os_sem_impl`, `os_msgqueue_impl`, `os_utils_impl`, `platform`, `cpputest_main`, `user_os_config`.

Three tasks exercise every user-facing scheduling macro at least once: `recv_task` (prio 1), `send_task` (prio 2), `ctrl_task` (prio 3). Macros covered: `task_open`, `task_close`, `task_wait`, `msg_receive`, `msg_receive_async`, `msg_post`, `msg_post_in`, `msg_post_every`, `msg_post_async`, `sem_wait`, `sem_signal`, `event_wait`, `event_signal`, `task_suspend`, `task_resume`.

An assert guard via `os_on_assert_attach_callback` catches any OS assert firing during the test run; the teardown fails if the guard trips.

Test name: `TEST(Integration, public_api_smoke_test)`. Tagged with `UT_CATALOG_ID("KERNEL-1")` and `UT_CATALOG_ID("KERNEL-2")`.

Added `test_integration` to both build preset target lists in `CMakePresets.json`. The test runner script (`scripts/test.py`) globs `*.elf` from the build directories, so it picks up `test_integration.elf` automatically.

---

## Phase 11 items — Coverage script, TASK-14 cleanup, event_wait_timeout_ex

### scripts/check_function_coverage.py (new)

New script that mechanically enforces the "every public function has a direct
test" rule. For each of the six components it reads the defined `T` symbols from
`build/x86_64/os/<comp>/lib<comp>_impl.a` via `nm -g --defined-only`, then
checks whether each symbol name appears as a direct call in
`tests/<comp>/src/<comp>.test.cpp`. Prints a green "All public functions are
directly tested" message on success or a red list of misses and exits non-zero.
Supports an `ALLOWLIST` set for symbols intentionally tested indirectly (currently
empty). CLI: `python3 scripts/check_function_coverage.py [--build-dir <path>]`.

### makefile

Added `check-coverage` target (installs termcolor if needed, then runs the script
against `build/x86_64`). Updated the `ci` target command from
`make build.all && make test && make check-trace` to also include `&& make check-coverage`.

### TASK-14 deleted (documents/requirements.csv + tests/os_task/src/os_task.test.cpp)

TASK-14 ("shall provide a function `task_ready_set`") described the file-static
internal `task_ready_set`, which is the same observable behavior already covered
by TASK-29 ("shall provide `os_task_ready_set`" -- the public function). Deleted
the CSV row and removed both `UT_CATALOG_ID("TASK-14")` tags (from
`release_task_prio_waiting_on_semaphore` and `task_waiting_semaphore`).
`check-trace` stays green; total requirement count is now 77.

### tests/integration/src/integration.test.cpp

Added `event_wait_timeout_ex_callback_and_early_signal` (EVENT-5, TASK-21):
the one remaining unexpanded user-facing macro. Creates a waiter task that calls
`event_wait_timeout_ex(evt, 10, cb)` and a signaler that fires at tick 4.
Asserts the callback fires during the first `unit_test_os_schedule()` call
(before the task yields), then asserts the task resumes with remaining timeout
ticks greater than zero (signaled early, not timed out).

---

## Phase 10 — Integration Smoke Test Macro Coverage

### tests/integration/src/integration.test.cpp

Nine new `TEST(Integration, ...)` cases added, each beginning with `os_init()` so they run independently of the three-task smoke-test fixture:

- `task_wait_id_driven_by_sub_clock` (KERNEL-6, KERNEL-15): creates a task that calls `task_wait_id(2, 3)` inside an infinite loop. Verifies that three master-clock ticks (`os_tick()`) do not advance sub-clock 2 and the task stays blocked, then that three `os_sub_tick(2)` calls wake it and increment a counter exactly once. A second assertion verifies `os_sub_nTick(2, 3)` produces the same result in one call.

- `event_wait_timeout_expires` (TASK-21, KERNEL-3): task calls `event_wait_timeout(evt, 5)` with no signaler. After five `os_tick()` calls the timeout fires; asserts the task ran past the macro and that `event_get_timeout()` returns 0 (remaining ticks = 0).

- `event_wait_timeout_early_signal` (TASK-21): waiter calls `event_wait_timeout(evt, 20)`, a second task calls `event_signal` after three ticks. Asserts `event_get_timeout()` returns a non-zero value (remaining ticks when signaled early).

- `event_wait_multiple_any` (EVENT-6): `event_wait_multiple(0, e1, e2)` wakes when e2 alone is signaled; asserts `event_last_signaled_get() == e2`.

- `event_wait_multiple_all` (EVENT-6): `event_wait_multiple(1, e1, e2)` stays blocked after e1 fires and wakes only after e2 fires; intermediate `CHECK_FALSE` confirms the "all" semantic.

- `event_ISR_signal_wakes_task` (EVENT-7, EVENT-8): `event_ISR_signal(evt)` makes the waiting task READY without an `OS_YIELD` in the caller; asserts `event_signaling_taskId_get(evt) == ISR_TID` (0xfe).

- `sem_ISR_signal_wakes_task` (TASK-11): `sem_ISR_signal(sem)` releases a task blocked on `sem_wait` without scheduling; asserts the task ran after the next `unit_test_os_schedule()`.

- `event_wait_ex_callback_fires_before_block` (EVENT-5): `event_wait_ex(evt, cb)` invokes the callback inside `os_wait_event` before `OS_YIELD`; asserts the callback flag is true after a single `unit_test_os_schedule()` even though the event has not been signaled yet.

- `msg_receive_ex_callback_fires_before_block` (EVENT-5): `msg_receive_ex(tid, &m, cb)` with an empty queue invokes the callback before blocking on the queue change event; asserts callback flag after a single `unit_test_os_schedule()`.

### makefile

Added `cpputest_Integration.xml` to the `--test` list in the `check-trace` target. Previously the integration test's JUnit XML was generated by the test runner but never read by `trace_reqs.py`; `UT_CATALOG_ID` tags in integration tests were invisible to the trace check.

---

## Phase 9 — Requirements Quality Fixes

### documents/requirements.csv

Full rewrite applying all findings from `documents/req-findings.md` (now deleted):

- **Column rename**: `test_case_id` → `requirement_id`. Updated `scripts/trace_reqs.py` in lockstep (`row['test_case_id']` → `row['requirement_id']`).
- **Style normalization**: Every row rewritten as a "shall" sentence. Product name normalized to `cocoOS` throughout (was also `CocoOS` / `CocoOs`).
- **TASK-1**: "bad data" replaced with the three concrete triggers: null task procedure, duplicate priority, exceeding N_TASKS.
- **TASK-2**: Added observable consequence — os_assert is triggered.
- **TASK-3 through TASK-16, TASK-18 through TASK-23**: Rewrote noun-phrase and terse descriptions as proper "shall" sentences naming the specific function.
- **TASK-13**: Updated function name from `task_wait_sem_set` to `os_task_wait_sem_set` to match the renamed implementation.
- **TASK-17 deleted**: Described the private `task_killed_set` function; observable behavior already covered by TASK-7. Removed the corresponding duplicate test `task_kill_traced_as_task17` from `tests/os_task/src/os_task.test.cpp`. Gaps in TASK numbering are intentional; `trace_reqs.py` does not require contiguity.
- **TASK-19**: "all tasks" corrected to "the specified task" — `os_task_clear_wait_queue` takes a single `tid`.
- **TASK-22**: Function name `task_tick` added to the description.
- **KERNEL-1**: "stops and the re-starts" → "suspends the current task and resumes its execution after the specified number of ticks".
- **KERNEL-2**: "tasks that do not execute only once" → "tasks without an infinite loop execute their body once and stop".
- **MSGQUEUE-10**: Clarified as the "all-delayed → EMPTY" case.
- **MSGQUEUE-11**: Clarified as the "skip delayed to deliver ready" case (ordering/skip behavior). Added an explicit assertion to `test_os_rcv` in `tests/os_msgqueue/src/os_msgqueue.test.cpp`: posts a delayed message followed by an immediate message, then asserts that `os_msg_receive` delivers the immediate one — demonstrating that earlier delayed messages are skipped.
- **EVENT-5**: "invokes the behaviors described by `os_task_wait_event`" cross-reference replaced with inline description: resets event signaling tid to NO_TID, puts task into WAITING_EVENT_TIMEOUT or WAITING_EVENT depending on timeout, executes callback if provided.
- **SEM-1**: Minor copy fix ("value or true or false" → "value of true or false").
- **EVENT-4**: Removed stray double-space.

Total requirement count: 78 rows (was 79; TASK-17 deleted). `check-trace` reports "All requirements are traced".

---

## Phase 8 — Explicit Function-Level Test Coverage

### Dead code removed

**os/os_task/include/os_task.h** — deleted declaration of `os_task_run_test` (had no definition anywhere) and `task_should_run_test` (orphaned test scaffolding).

**os/os_task/src/os_task.c** — deleted the body of `task_should_run_test` (the `state==0 || state==99` magic) and `task_set_no_running_task` (no declaration, no callers).

**os/os_task/mocks/src/mock_os_task.c** — removed mock entries for `os_task_run_test` and `task_should_run_test`.

### New os_task tests (TASK-26 through TASK-31)

Six tests added to `tests/os_task/src/os_task.test.cpp`:

- `wait_queue_accessor_pair` (TASK-26): `os_task_set_wait_queue` / `os_task_get_wait_queue` round-trips on two tasks without interference.
- `change_event_accessor_pair` (TASK-27): `os_task_set_change_event` / `os_task_get_change_event` round-trips.
- `msg_result_accessor_pair` (TASK-28): `os_task_set_msg_result` / `os_task_get_msg_result` round-trips.
- `ready_set_puts_task_in_ready_state` (TASK-29): suspends a task then calls `os_task_ready_set`; asserts state == READY.
- `internal_state_set_and_get` (TASK-30): stores two different values via `os_task_internal_state_set` and reads back each; covers OS_YIELD re-entry semantics.
- `task_run_executes_running_task_proc` (TASK-31): sets a flag-setting proc, installs the running tid via the kernel mock, calls `task_run()`, asserts the proc ran.

A static `flag_task` / `s_flag_task_ran` pair was added at the top of the test file for the TASK-31 test.

### os_on_assert fix (production bug)

**os/os_utils/src/os_assert.c** — added `return;` after `user_callback(file, line, expr)`. When a callback is attached, `os_on_assert` now returns instead of falling into `while(1)`. This is the agreed contract: attaching a callback converts the halt into a notify/return, enabling testability. Without a callback the behavior is unchanged (still halts).

This also fixes the integration test's assert guard, which previously set a flag and returned from the callback but then fell into `while(1)` and hung QEMU CI.

### NASSERT fix

**os/os_utils/include/os_assert.h** — added `#define os_assert_with_return( test, rtn )` to the `#else` (NASSERT) branch. Previously only `os_assert` was defined there; any TU using `os_assert_with_return` with NASSERT would fail to compile.

### New test suite: tests/os_utils/

New target `test_os_utils` added under `tests/os_utils/`:

- `tests/os_utils/CMakeLists.txt` — links real `os_utils_impl` + `os_kernel_impl_mock` (needed to satisfy `os_tick` from `system_time.c`) + `platform` + `cpputest_main` + `user_os_config`.
- `tests/os_utils/src/os_utils.test.cpp` — four tests:
  - `assert_with_callback_invokes_and_returns` (UTILS-1, UTILS-2): attaches callback, calls `os_on_assert`, checks all three arguments forwarded and function returns.
  - `callback_replacement_fires_only_new_callback` (UTILS-2): replaces callback, asserts only the new one fires.
  - `os_assert_macro_triggers_on_false_noop_on_true` (UTILS-1): calls helper that invokes `os_assert(false)` and checks callback fired; calls helper that invokes `os_assert(true)` and checks callback did not fire.
  - `os_assert_with_return_fires_and_returns_value` (UTILS-3): helper returning int calls `os_assert_with_return(false, 7)`; checks return value is 7 and callback fired.

### Wire-up

- `tests/CMakeLists.txt` — added `add_subdirectory(os_utils)`.
- `CMakePresets.json` — added `test_os_utils` to both `x86_64` and `a9` build preset target lists.
- `makefile` — added `cpputest_TestOsUtils.xml` to the `check-trace` XML list.

### New requirements (documents/requirements.csv)

Added 9 rows: TASK-26 through TASK-31 and UTILS-1 through UTILS-3. All are immediately traced by the new tests above. Total requirement count is now 79. `check-trace` reports "All requirements are traced".

---

## CI Workflow

`.github/workflows/main.yml`:

- Updated `actions/checkout@v2` to `actions/checkout@df4cb1c069e1874edd31b4311f1884172cec0e10` (v6.0.3 pinned by commit hash).
- Added a weekly cron trigger (`0 0 * * 0`, Sunday midnight UTC) in addition to the existing on-push trigger.
