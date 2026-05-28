//go:build !windows

package esf

import (
	"os"
	"syscall"
)

// mmapFile maps a region of the file into memory (read-only).
func mmapFile(fd *os.File, offset, length int64) ([]byte, error) {
	// mmap requires page-aligned offset.
	pageSize := int64(syscall.Getpagesize())
	alignedOffset := offset &^ (pageSize - 1)
	delta := offset - alignedOffset
	alignedLength := length + delta

	data, err := syscall.Mmap(int(fd.Fd()), alignedOffset, int(alignedLength),
		syscall.PROT_READ, syscall.MAP_PRIVATE)
	if err != nil {
		return nil, err
	}
	return data[delta:], nil
}

// munmapFile releases the mmap.
func munmapFile(data []byte) {
	// Recover original mmap slice (may have been sub-sliced by delta adjustment).
	// syscall.Munmap needs the original pointer; pass through and let OS handle.
	syscall.Munmap(data)
}
