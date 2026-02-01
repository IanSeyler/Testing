Testing repo

## syscall_test (getpid)

### Physical

- AMD RYZEN AI MAX+ 395 w/ Radeon™ 8060S × 32
- 128.0 GiB RAM
- Ubuntu 25.10

```
Iterations: 1000000
Min: 0 ns
Max: 32932 ns
Average: 72.01 ns
```

### Virtual

- Via KVM on physical host
- 4.0 GiB RAM
- Ubuntu 25.10

```
Iterations: 1000000
Min: 0 ns
Max: 35598 ns
Average: 76.75 ns
```

## ethernet_test

### Physical

Specs as above

```
Iterations: 1000000 (warmup excluded: 10000)
Buffer: 2048 bytes
Total bytes read (successful): 2976228
Other errors (excluded from stats): 0
recvfrom() returned PACKET: count=2775
 Min=291 ns
 Max=3126 ns
 Avg=451.91 ns
recvfrom() returned NO PACKET (EAGAIN): count=987225
 Min=130 ns
 Max=6132 ns
 Avg=141.66 ns
```

### Virtual

Specs as above

```
Iterations: 1000000 (warmup excluded: 10000)
Buffer: 2048 bytes
Total bytes read (successful): 1111200
Other errors (excluded from stats): 0
recvfrom() returned PACKET: count=946
 Min=210 ns
 Max=13997 ns
 Avg=450.52 ns
recvfrom() returned NO PACKET (EAGAIN): count=989054
 Min=90 ns
 Max=181300 ns
 Avg=102.48 ns
```
