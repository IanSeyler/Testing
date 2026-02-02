Testing repo

# Linux

## syscall test - `getpid_bench.c`

### System 1

- Framework Desktop
- AMD RYZEN AI MAX+ 395 w/ Radeon™ 8060S × 32
- 128.0 GiB RAM
- Ubuntu 25.10

#### Physical

```
Iterations: 1000000
Min: 0 ns
Max: 32932 ns
Average: 72.01 ns
```

#### Virtual

- Via KVM on physical host
- 4.0 GiB RAM
- Ubuntu 25.10

```
Iterations: 1000000
Min: 0 ns
Max: 35598 ns
Average: 76.75 ns
```

### System 2

- Framework Laptop
- 11th Gen Intel® Core™ i5-1135G7 × 8
- 64.0 GiB RAM
- Ubuntu 25.10

#### Physical

```
Iterations: 1000000
Min: 0 ns
Max: 54742 ns
Average: 301.41 ns
```

#### Virtual

- Via KVM on physical host
- 4.0 GiB RAM
- Ubuntu 25.10

```
Iterations: 1000000
Min: 0 ns
Max: 43831 ns
Average: 324.15 ns
```

## ethernet test - `ethernet_bench.c`

### System 1

Specs as above

#### Physical

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

#### Virtual

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

### System 2

Specs as above

#### Physical

```
Iterations: 1000000 (warmup excluded: 10000)
Buffer: 2048 bytes
Total bytes read (successful): 646721
Other errors (excluded from stats): 0
recvfrom() returned PACKET: count=608
 Min=1288 ns
 Max=8239 ns
 Avg=2681.97 ns
recvfrom() returned NO PACKET (EAGAIN): count=989392
 Min=412 ns
 Max=26648 ns
 Avg=488.27 ns
```

#### Virtual

Specs as above

```
Iterations: 1000000 (warmup excluded: 10000)
Buffer: 2048 bytes
Total bytes read (successful): 467760
Other errors (excluded from stats): 0
recvfrom() returned PACKET: count=429
 Min=1349 ns
 Max=14551 ns
 Avg=1976.63 ns
recvfrom() returned NO PACKET (EAGAIN): count=989571
 Min=449 ns
 Max=133626 ns
 Avg=492.44 ns
```

# BareMetal

## syscall test - `sys_test.asm`

### System 2

- Framework Laptop
- 11th Gen Intel® Core™ i5-1135G7 × 8
- 64.0 GiB RAM
- BareMetal 2026.01

#### Physical

```
32 ns
```
