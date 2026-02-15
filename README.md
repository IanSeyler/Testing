Testing repo

# Virtual via KVM

Physical system for testing is as follows:

- Framework Desktop
- AMD RYZEN AI MAX+ 395 w/ Radeon™ 8060S × 32
- 128.0 GiB RAM
- Ubuntu 25.10

## l_bench / b_bench

Executing `cpuid` instruction in a loop.

### Linux (Ubuntu 25.10)

```qemu-system-x86_64 -machine q35 -name "Ubuntu 25.10" -smp sockets=1,cpus=4 -cpu host -enable-kvm -m 4096 -drive id=disk0,file=ubuntu2510.img,if=none,format=raw -device virtio-scsi-pci -device scsi-hd,drive=disk0 -netdev user,id=nat0 -device e1000,netdev=nat0 -netdev socket,id=priv0,listen=:12345 -device virtio-net-pci,netdev=priv0```

```
ian@ubuntu2510:~/Code/Testing$ ./l_bench
Starting benchmark...

--- Results ---
Iterations: 1000000
Average: 2270.05 ns
ian@ubuntu2510:~/Code/Testing$ ./l_bench
Starting benchmark...

--- Results ---
Iterations: 1000000
Average: 2269.85 ns
ian@ubuntu2510:~/Code/Testing$ ./l_bench
Starting benchmark...

--- Results ---
Iterations: 1000000
Average: 2286.01 ns
ian@ubuntu2510:~/Code/Testing$
```

### BareMetal (2026.01)

```qemu-system-x86_64 -machine q35 -name "BareMetal OS" -smp sockets=1,cpus=4 -cpu host -enable-kvm -m 256 -drive id=disk0,file="sys/baremetal_os.img",if=none,format=raw -device ide-hd,drive=disk0 -netdev socket,id=testnet1,connect=localhost:12345 -device virtio-net-pci,netdev=testnet1,mac=10:11:12:00:1A:F4```

```
> load
Enter file number: 4
> exec
Starting benchmark...

--- Results ---
Iterations: 1000000
Average: 2277 ns
> exec
Starting benchmark...

--- Results ---
Iterations: 1000000
Average: 2276 ns
> exec
Starting benchmark...

--- Results ---
Iterations: 1000000
Average: 2279 ns
```

### Summary

This verified that the benchmarking tool is working correctly on both Linux and BareMetal OS.

## l_ethernet_bench / b_ethernet_bench
