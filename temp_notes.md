* I broke out the library into their own logical components to better
  facilitate testing. All of the libraries pulled in the parent include file
  `cocoos.h`, which is convenient because you don't have to keep track of
  dependencies. However, it makes it very difficult to test because if all of
  the components have a many-to-many relationship they can't be split apart to
  test them independently.

  As such I think it makes more sense to split up the design into discrete
  logical components that can be addressed individually.

* Consider the usage of stdbool.h and whether it's compatible with all
  platforms

* I think the library configs should be a file provided by the user, though I
  need to think about the implications of how to do this with cmake (or not
  cmake if the useres opt not to use it). Splitting the library into logical
  components begets testing, but doesn't share the global defines like was
  orginally expected.

* I've been removing the lic code to make it more readable.

* Changed Sem_t backing type to be int16 to allow for negative numbers

* The scheduling macros are clever. On re-entry into the function the "last
  saved state" is function of the line number (it's really not important what
  the number is), only that it doesn't conflict with a valid state like READY.

  On re-entry into the function after the first time, the saved state will be
  the same. That's also why the for loop is crucial, b/c you won't be in a
  READY state, but the for loop jumps you up to the top of the case. I have an
  example of the expanded macros [here](https://godbolt.org/z/z4vPd5Wh7). This
  is really clever and simulates the effect of having a `for(;;)` inside a task
  which makes people comfortable and is brilliant. Over-all the idea is quite
  good.

  What I _don't_ like about it is:
  * It's very difficult to tell what's going on. It breaks down to something
    like below. There's a for loop broken up by a case statement, semi-colons
    in weird places. I'm no the fence if I should leave it like this and simply
    document the behavior, or if I should make it something more predictable /
    understandable.

  ```c
      switch ( os_task_state )
    {
        case 0:;
               for(;;)
               {
                   printf("foo %d\n", os_task_state);
                   do {
                        printf("inside_do %d\n", os_task_state);
                        return;
                        case (44 +0):;
                   } while ( 0 );
               }

            printf("done task\n")
  ```

  * The internal state being dependent on line-number I fear will cause
    problems, especially if a task conflicted with a valid state.

  * I'm worried some optimizer will see the expanded macros and say "uh, no",
    or more likely, static analyais will yell b/c of this crazy formulation.

  * `running_tid` ends up being a global variable which can be manipulated by
    _anyone_, which seems bad. This is becuase the macros end up expanding to
    `uint16_t os_task_state = task_internal_state_get(running_tid);` in order
    to get the `internal_state` of _this_ task. Simulating OOP for this small
    part would probably be safer if possible.

  * The macros create variables with names, so if you were unlikely and chose
    the same variable name, it would fail to compile but in a really weird way.

  * ~~There's a bug which is for the _first_ run in a `for(;;)` definition, if
    you have any code _after_ a `task_wait` it will never get called (b/c you
    start in case 0) and jump to return as soon as you set the wait state.~~
    This isn't the case, it works as expected.

  * I'd like to turn this into something MISRA compliant, and the heavy marcro
    usage violates
    > Misra rule 19.7 : A function should be used in preference to a function-like macro
    I think removing or at least removing the heavy macro usage will allow for
    something more robust, safer, and test-able. All of this text subsitution
    is a strong code smell, even though I do think it's really clever. I think
    there's some control flow implementation that will probably achieve
    something similar

  * I'm starting to really understand why these macros leverage `switch`
    statements and not `goto` labels. Using the LINE lets you create an
    arbitrary state (which I knew), but it's numbered, not named. It also lets
    you add as many scheduling macros as you'd like, b/c it just adds a case
    which is dependent on the __LINE__. My 2 concerns are 1. w/o using `{}` in
    the switch statements, will the compiler complain about a variable
    decleration between scheduling macros? 2. What if __LINE__ conflicts with
    an actual case used by the user in a switch statement, that's a very
    difficult bug to track down.

    ~~Even if the compiler doesn't complain, the below will print 0, b/c x is
    valid and scoped to the switch statement (which is fine), but let's say we
    re-enter on case 2, previously having entered case 1...you're going to have
    a bad time b/c x is going to be wrong, b/c the previous value of x was
    pop'd off the stack...and yet here you are using it. This can probably be
    fixed by adding an explicit variable store/restore function that the user
    opts to use to store variables between re-entry.~~ This is a known
    limitation of the implementation. The solutionis to decalre all of your
    variables static, or, pass the task a pointer to data that you're going to
    want to access.

    ```
       switch(2)
    {
        case 0:
        case 1:
            char x = 10;
            printf("hello\n");
        case 2:
            printf("%d\n", x);
        default:
            break;
    }
    ```

    * After a ton of heart-ache, I've figured out that this is  Duff's Device
      for approximating re-entrant code whilst abusing the C macro. As such,
      I'm not going to change the program flow, though I am going to change a
      few things.

    * __LINE__+ofs isn't offering much in the macros, it just makes things a
      bit more confusing I'd argue, so I removed them for now

    * `os_start` now takes an argument to let an infinite loop run
