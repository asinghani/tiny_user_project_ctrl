import serial, sys

ser = serial.Serial(sys.argv[1], 9600, timeout=0)

ser.timeout = 0
ser.read(99999)
ser.timeout = 10

input_bit = 0

# sets the inputs and reads the outputs
# if you just want to read outputs, then
# just feed the same inputs as last time
def do_xfer(inputs):
    assert len(inputs) == 8
    x = 0
    for i in range(8):
        x = (x << 1) | (1 if inputs[::-1][i] else 0)

    ser.write(bytearray([x]))
    ser.flush()

    dat = ser.read(1)
    assert len(dat) == 1
    arr = [1 if (dat[0] & (1 << i)) else 0 for i in range(8)]
    return arr

# Set reset, step clock
for i in range(10):
    print(do_xfer([i%2,1,0,0,0,0,0,0]))

# Unset reset, step clock
for i in range(10):
    print(do_xfer([i%2,0,0,0,0,0,0,0]))

# Press button, step clock
for i in range(10):
    print(do_xfer([i%2,0,1,0,0,0,0,0]))

# Unpress button, step clock for a long time
for i in range(1000000000):
    print(do_xfer([i%2,0,0,0,0,0,0,0]))
