window.BENCHMARK_DATA = {
  "lastUpdate": 1761309921845,
  "repoUrl": "https://github.com/aka411/tlsf-memory-allocator",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "181660723+aka411@users.noreply.github.com",
            "name": "Akash Joshi",
            "username": "aka411"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "cba6242c5edd11b37cd41df5d8794bc9c3847046",
          "message": "Refactor benchmark code for TLSF allocator",
          "timestamp": "2025-10-24T18:12:24+05:30",
          "tree_id": "e616fc2f185c8c2e47f6f3898ef155d1c8c15153",
          "url": "https://github.com/aka411/tlsf-memory-allocator/commit/cba6242c5edd11b37cd41df5d8794bc9c3847046"
        },
        "date": 1761309921313,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "SystemMalloc_AllocateAndFree/900",
            "value": 10.577805244338633,
            "unit": "ns/iter",
            "extra": "iterations: 65806888\ncpu: 10.576845876680874 ns\nthreads: 1"
          },
          {
            "name": "SystemMalloc_AllocateAndFree/4096",
            "value": 23.120217462197157,
            "unit": "ns/iter",
            "extra": "iterations: 29904416\ncpu: 23.117087957845428 ns\nthreads: 1"
          },
          {
            "name": "SystemMalloc_AllocateAndFree/40960",
            "value": 24.884413743668425,
            "unit": "ns/iter",
            "extra": "iterations: 28130161\ncpu: 24.88325999982724 ns\nthreads: 1"
          },
          {
            "name": "SystemMalloc_AllocateAndFree/524288",
            "value": 24.263897153945727,
            "unit": "ns/iter",
            "extra": "iterations: 28869868\ncpu: 24.26120292617895 ns\nthreads: 1"
          },
          {
            "name": "SystemMalloc_AllocateAndFree/1048576",
            "value": 24.29977825206158,
            "unit": "ns/iter",
            "extra": "iterations: 28845815\ncpu: 24.29815295563671 ns\nthreads: 1"
          },
          {
            "name": "MyTLSFAllocatorFixture/MyTlsf_AllocateAndFree/900",
            "value": 331.62850442242836,
            "unit": "ns/iter",
            "extra": "iterations: 2108122\ncpu: 331.604212185063 ns\nthreads: 1"
          },
          {
            "name": "MyTLSFAllocatorFixture/MyTlsf_AllocateAndFree/4096",
            "value": 331.4871925501856,
            "unit": "ns/iter",
            "extra": "iterations: 2112052\ncpu: 331.466324692763 ns\nthreads: 1"
          },
          {
            "name": "MyTLSFAllocatorFixture/MyTlsf_AllocateAndFree/40960",
            "value": 331.5768487329277,
            "unit": "ns/iter",
            "extra": "iterations: 2110770\ncpu: 331.55576590533286 ns\nthreads: 1"
          },
          {
            "name": "MyTLSFAllocatorFixture/MyTlsf_AllocateAndFree/524288",
            "value": 332.09440753059005,
            "unit": "ns/iter",
            "extra": "iterations: 2111071\ncpu: 332.0686182511151 ns\nthreads: 1"
          },
          {
            "name": "MyTLSFAllocatorFixture/MyTlsf_AllocateAndFree/1048576",
            "value": 331.46947553394284,
            "unit": "ns/iter",
            "extra": "iterations: 2110127\ncpu: 331.4333980845702 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}