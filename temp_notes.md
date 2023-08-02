* I broke out the library into their own logical components to better
  facilitate testing. All of the libraries pulled in the parent include file
  `cocoos.h`, which is convenient because you don't have to keep track of
  dependencies. However, it makes it very difficult to test because if all of
  the components have a many-to-many relationship they can't be split apart to
  test them independently.

  As such I think it makes more sense to split up the design into discrete
  logical components that can be addressed individually.

```dot
digraph "test" {
node [
  fontsize = "12"
];
    "node19" [ label = "test", shape = egg ];
    "node0" [ label = "CppUTest\n(CppUTest::CppUTest)", shape = octagon ];
    "node19" -> "node0" [ style = dotted ] // test -> CppUTest
    "node1" [ label = "-pthread", shape = septagon ];
    "node0" -> "node1" [ style = dotted ] // CppUTest -> -pthread
    "node2" [ label = "CppUTestExt\n(CppUTest::CppUTestExt)", shape = octagon ];
    "node19" -> "node2" [ style = dotted ] // test -> CppUTestExt
    "node0" [ label = "CppUTest\n(CppUTest::CppUTest)", shape = octagon ];
    "node2" -> "node0"  // CppUTestExt -> CppUTest
    "node7" [ label = "os_kernel_impl", shape = octagon ];
    "node19" -> "node7" [ style = dotted ] // test -> os_kernel_impl
    "node8" [ label = "os_kernel_intf", shape = pentagon ];
    "node7" -> "node8"  // os_kernel_impl -> os_kernel_intf
    "node9" [ label = "os_task_intf", shape = pentagon ];
    "node7" -> "node9"  // os_kernel_impl -> os_task_intf
    "node6" [ label = "os_event_intf", shape = pentagon ];
    "node9" -> "node6" [ style = dashed ] // os_task_intf -> os_event_intf
    "node10" [ label = "os_msgqueue_intf", shape = pentagon ];
    "node9" -> "node10" [ style = dashed ] // os_task_intf -> os_msgqueue_intf
    "node11" [ label = "os_sem_intf", shape = pentagon ];
    "node9" -> "node11" [ style = dashed ] // os_task_intf -> os_sem_intf
    "node12" [ label = "os_utils_intf", shape = pentagon ];
    "node9" -> "node12" [ style = dashed ] // os_task_intf -> os_utils_intf
    "node14" [ label = "os_sem_impl", shape = octagon ];
    "node19" -> "node14" [ style = dotted ] // test -> os_sem_impl
    "node11" [ label = "os_sem_intf", shape = pentagon ];
    "node14" -> "node11"  // os_sem_impl -> os_sem_intf
    "node15" [ label = "os_task_impl", shape = octagon ];
    "node19" -> "node15" [ style = dotted ] // test -> os_task_impl
    "node11" [ label = "os_sem_intf", shape = pentagon ];
    "node15" -> "node11"  // os_task_impl -> os_sem_intf
    "node9" [ label = "os_task_intf", shape = pentagon ];
    "node15" -> "node9"  // os_task_impl -> os_task_intf
    "node12" [ label = "os_utils_intf", shape = pentagon ];
    "node15" -> "node12"  // os_task_impl -> os_utils_intf
    "node16" [ label = "os_utils_impl", shape = octagon ];
    "node19" -> "node16" [ style = dotted ] // test -> os_utils_impl
    "node8" [ label = "os_kernel_intf", shape = pentagon ];
    "node16" -> "node8"  // os_utils_impl -> os_kernel_intf
    "node12" [ label = "os_utils_intf", shape = pentagon ];
    "node16" -> "node12"  // os_utils_impl -> os_utils_intf
    "node17" [ label = "platform", shape = octagon ];
    "node19" -> "node17" [ style = dotted ] // test -> platform
    "node18" [ label = "os_kernel", shape = septagon ];
    "node17" -> "node18" [ style = dotted ] // platform -> os_kernel
    "node19" [ label = "test", shape = egg ];
    "node19" -> "node19"  // test -> test
}
```
