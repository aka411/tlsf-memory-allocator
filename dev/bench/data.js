window.BENCHMARK_DATA = {
  "lastUpdate": 1761255580174,
  "repoUrl": "https://github.com/aka411/tlsf-memory-allocator",
  "entries": {
    "^AllocatorFixture.*": [
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
          "id": "2b8a6f115bf3b8303f4b81c4b21def6c0eebf9d3",
          "message": "Fix formatting and clean up CMakeLists.txt",
          "timestamp": "2025-10-24T03:07:07+05:30",
          "tree_id": "761b05531c14526aeadc5f8443a8b8ed958e0577",
          "url": "https://github.com/aka411/tlsf-memory-allocator/commit/2b8a6f115bf3b8303f4b81c4b21def6c0eebf9d3"
        },
        "date": 1761255577653,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_System_Malloc/40960",
            "value": 25.35545311412819,
            "unit": "ns/iter",
            "extra": "iterations: 27679991\ncpu: 25.35331705852072 ns\nthreads: 1"
          },
          {
            "name": "AllocatorFixture/BM_Tlsf_AllocateAndFree/40960",
            "value": 332.86324477332516,
            "unit": "ns/iter",
            "extra": "iterations: 2096695\ncpu: 332.85714326594945 ns\nthreads: 1"
          }
        ]
      }
    ],
    "^BM_System_Malloc.*": [
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
          "id": "2b8a6f115bf3b8303f4b81c4b21def6c0eebf9d3",
          "message": "Fix formatting and clean up CMakeLists.txt",
          "timestamp": "2025-10-24T03:07:07+05:30",
          "tree_id": "761b05531c14526aeadc5f8443a8b8ed958e0577",
          "url": "https://github.com/aka411/tlsf-memory-allocator/commit/2b8a6f115bf3b8303f4b81c4b21def6c0eebf9d3"
        },
        "date": 1761255579477,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_System_Malloc/40960",
            "value": 25.35545311412819,
            "unit": "ns/iter",
            "extra": "iterations: 27679991\ncpu: 25.35331705852072 ns\nthreads: 1"
          },
          {
            "name": "AllocatorFixture/BM_Tlsf_AllocateAndFree/40960",
            "value": 332.86324477332516,
            "unit": "ns/iter",
            "extra": "iterations: 2096695\ncpu: 332.85714326594945 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}