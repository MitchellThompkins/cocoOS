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
subgraph clusterLegend {
  label = "Legend";
  color = black;
  edge [ style = invis ];
  legendNode0 [ label = "Executable", shape = egg ];
  legendNode1 [ label = "Static Library", shape = octagon ];
  legendNode2 [ label = "Shared Library", shape = doubleoctagon ];
  legendNode3 [ label = "Module Library", shape = tripleoctagon ];
  legendNode4 [ label = "Interface Library", shape = pentagon ];
  legendNode5 [ label = "Object Library", shape = hexagon ];
  legendNode6 [ label = "Unknown Library", shape = septagon ];
  legendNode7 [ label = "Custom Target", shape = box ];
  legendNode0 -> legendNode1 [ style = solid ];
  legendNode0 -> legendNode2 [ style = solid ];
  legendNode0 -> legendNode3;
  legendNode1 -> legendNode4 [ label = "Interface", style = dashed ];
  legendNode2 -> legendNode5 [ label = "Private", style = dotted ];
  legendNode3 -> legendNode6 [ style = solid ];
  legendNode0 -> legendNode7;
}
    "node0" [ label = "CppUTest\n(CppUTest::CppUTest)", shape = octagon ];
    "node1" [ label = "-pthread", shape = septagon ];
    "node0" -> "node1" [ style = dotted ] // CppUTest -> -pthread
    "node2" [ label = "CppUTestExt\n(CppUTest::CppUTestExt)", shape = octagon ];
    "node2" -> "node0"  // CppUTestExt -> CppUTest
    "node3" [ label = "mocks", shape = octagon ];
    "node3" -> "node0" [ style = dotted ] // mocks -> CppUTest
    "node3" -> "node2" [ style = dotted ] // mocks -> CppUTestExt
    "node4" [ label = "os_event_impl", shape = octagon ];
    "node5" [ label = "os_utils", shape = septagon ];
    "node4" -> "node5" [ style = dotted ] // os_event_impl -> os_utils
    "node6" [ label = "os_event_intf", shape = pentagon ];
    "node7" [ label = "os_kernel_impl", shape = octagon ];
    "node8" [ label = "os_kernel_intf", shape = pentagon ];
    "node9" [ label = "os_msgqueue_intf", shape = pentagon ];
    "node9" -> "node6" [ style = dashed ] // os_msgqueue_intf -> os_event_intf
    "node8" -> "node9" [ style = dashed ] // os_kernel_intf -> os_msgqueue_intf
    "node10" [ label = "os_sem_intf", shape = pentagon ];
    "node8" -> "node10" [ style = dashed ] // os_kernel_intf -> os_sem_intf
    "node11" [ label = "os_task_intf", shape = pentagon ];
    "node11" -> "node6" [ style = dashed ] // os_task_intf -> os_event_intf
    "node11" -> "node9" [ style = dashed ] // os_task_intf -> os_msgqueue_intf
    "node11" -> "node10" [ style = dashed ] // os_task_intf -> os_sem_intf
    "node12" [ label = "os_utils_intf", shape = pentagon ];
    "node11" -> "node12" [ style = dashed ] // os_task_intf -> os_utils_intf
    "node8" -> "node11" [ style = dashed ] // os_kernel_intf -> os_task_intf
    "node7" -> "node8"  // os_kernel_impl -> os_kernel_intf
    "node7" -> "node11"  // os_kernel_impl -> os_task_intf
    "node13" [ label = "os_utils_impl", shape = octagon ];
    "node13" -> "node8"  // os_utils_impl -> os_kernel_intf
    "node13" -> "node12"  // os_utils_impl -> os_utils_intf
    "node7" -> "node13"  // os_kernel_impl -> os_utils_impl
    "node7" -> "node12"  // os_kernel_impl -> os_utils_intf
    "node14" [ label = "os_msgqueue_impl", shape = octagon ];
    "node14" -> "node9"  // os_msgqueue_impl -> os_msgqueue_intf
    "node15" [ label = "os_sem_impl", shape = octagon ];
    "node15" -> "node10"  // os_sem_impl -> os_sem_intf
    "node16" [ label = "os_task_impl", shape = octagon ];
    "node16" -> "node8"  // os_task_impl -> os_kernel_intf
    "node16" -> "node10"  // os_task_impl -> os_sem_intf
    "node16" -> "node11"  // os_task_impl -> os_task_intf
    "node16" -> "node12"  // os_task_impl -> os_utils_intf
    "node17" [ label = "platform", shape = octagon ];
    "node17" -> "node8" [ style = dotted ] // platform -> os_kernel_intf
    "node18" [ label = "test", shape = egg ];
    "node18" -> "node0" [ style = dotted ] // test -> CppUTest
    "node18" -> "node2" [ style = dotted ] // test -> CppUTestExt
    "node18" -> "node7" [ style = dotted ] // test -> os_kernel_impl
    "node18" -> "node15" [ style = dotted ] // test -> os_sem_impl
    "node18" -> "node16" [ style = dotted ] // test -> os_task_impl
    "node18" -> "node13" [ style = dotted ] // test -> os_utils_impl
    "node18" -> "node17" [ style = dotted ] // test -> platform
    "node18" -> "node18"  // test -> test
}
```
