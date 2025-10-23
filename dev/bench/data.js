window.BENCHMARK_DATA = {
  "lastUpdate": 1761251611360,
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
          "id": "2d31370aac1c38ba2029966e4af7f0b142ea3ccd",
          "message": "Update CI workflow to enable deployments and auto-push",
          "timestamp": "2025-10-24T01:06:43+05:30",
          "tree_id": "66f785fe6ff15219d4f7ba20f25f2c017fb26747",
          "url": "https://github.com/aka411/tlsf-memory-allocator/commit/2d31370aac1c38ba2029966e4af7f0b142ea3ccd"
        },
        "date": 1761248348446,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_System_Malloc/40960",
            "value": 25.347540821771208,
            "unit": "ns/iter",
            "extra": "iterations: 27613513\ncpu: 25.345455212453416 ns\nthreads: 1"
          },
          {
            "name": "AllocatorFixture/BM_Tlsf_AllocateAndFree/40960",
            "value": 329.7429162445699,
            "unit": "ns/iter",
            "extra": "iterations: 2122250\ncpu: 329.73165461185056 ns\nthreads: 1"
          }
        ]
      },
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
          "id": "2d31370aac1c38ba2029966e4af7f0b142ea3ccd",
          "message": "Update CI workflow to enable deployments and auto-push",
          "timestamp": "2025-10-24T01:06:43+05:30",
          "tree_id": "66f785fe6ff15219d4f7ba20f25f2c017fb26747",
          "url": "https://github.com/aka411/tlsf-memory-allocator/commit/2d31370aac1c38ba2029966e4af7f0b142ea3ccd"
        },
        "date": 1761248476501,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_System_Malloc/40960",
            "value": 131.63453653416707,
            "unit": "ns/iter",
            "extra": "iterations: 4977778\ncpu: 131.83593161446734 ns\nthreads: 1"
          },
          {
            "name": "AllocatorFixture/BM_Tlsf_AllocateAndFree/40960",
            "value": 136.75682201978958,
            "unit": "ns/iter",
            "extra": "iterations: 4977778\ncpu: 131.83593161446734 ns\nthreads: 1"
          }
        ]
      }
    ],
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
          "id": "195b13a283346f199ff65156ea2fcf0d61eb5768",
          "message": "Add performance chart steps for TLSF and System Malloc\n\nUpdated benchmark action steps to include TLSF Allocator and System Malloc performance charts with specific filtering.",
          "timestamp": "2025-10-24T02:01:06+05:30",
          "tree_id": "e40468b5bccb55c7c5104d69d9223b5c957d5915",
          "url": "https://github.com/aka411/tlsf-memory-allocator/commit/195b13a283346f199ff65156ea2fcf0d61eb5768"
        },
        "date": 1761251611004,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_System_Malloc/40960",
            "value": 25.281482325178803,
            "unit": "ns/iter",
            "extra": "iterations: 27550689\ncpu: 25.27784695330124 ns\nthreads: 1"
          },
          {
            "name": "AllocatorFixture/BM_Tlsf_AllocateAndFree/40960",
            "value": 333.86317446761495,
            "unit": "ns/iter",
            "extra": "iterations: 2096005\ncpu: 333.81922705337064 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}