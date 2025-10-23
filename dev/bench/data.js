window.BENCHMARK_DATA = {
  "lastUpdate": 1761248348839,
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
      }
    ]
  }
}