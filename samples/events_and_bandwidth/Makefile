include ../config.mk

EXECUTABLE=events_and_bandwidth
SOURCES=$(EXECUTABLE).cpp
CUDA_SRC=$(EXECUTABLE)_cuda.cu

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDE) $(LDFLAGS)

cuda: $(CUDA_SRC)
	$(NVCC) $^ -o $(EXECUTABLE)_$@

.PHONY: clean

clean:
	rm -f $(EXECUTABLE) $(EXECUTABLE)_cuda