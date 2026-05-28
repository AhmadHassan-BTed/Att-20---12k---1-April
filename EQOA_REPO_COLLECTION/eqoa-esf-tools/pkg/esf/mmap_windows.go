//go:build windows

package esf

import (
	"fmt"
	"os"
)

// mmapFile is not implemented on Windows — falls back to heap read.
func mmapFile(fd *os.File, offset, length int64) ([]byte, error) {
	return nil, fmt.Errorf("mmap not implemented on Windows")
}

func munmapFile(data []byte) {}
