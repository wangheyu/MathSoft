import ctypes

lib = ctypes.CDLL("./libgsl_wrapper.so")

lib.my_bessel_j0.argtypes = [ctypes.c_double]
lib.my_bessel_j0.restype = ctypes.c_double

x = 5.0
y = lib.my_bessel_j0(x)

print(f"J0({x}) = {y:.18e}")
