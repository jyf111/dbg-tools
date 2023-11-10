if __name__ == "__main__":
  import sys
  if len(sys.argv) < 2:
    print("Please input the number of args to generate!")
  else:
    print("#define _DBG_CONCAT(x,y) x ## y")
    print("#define _DBG_CONCAT_HELPER(x,y) _DBG_CONCAT(x,y)")
    print("#define _DBG_GET_NTH_ARG(", end="")
    num = int(sys.argv[1])
    for i in range(num):
      print(f"_{i + 1},", end="")
    print("N,...) N")
    print("#define _DBG_COUNT_ARGS(...) _DBG_GET_NTH_ARG(__VA_ARGS__", end="")
    for i in range(num, -1, -1):
      print(f",{i}", end="")
    print(")")
    for i in range(0, num + 1):
      print(f"#define _DBG_APPLY_F{i}(f", end="")
      if i == 0:
        print(")")
      elif i == 1:
        print(",_1) f(_1)")
      else:
        print(f",_1,...) f(_1)_DBG_APPLY_F{i - 1}(f,__VA_ARGS__)")
    print("#define _DBG_APPLY_HELPER(f,...) f(__VA_ARGS__)")
    print("#define _DBG_FOR_EACH(f,...) _DBG_APPLY_HELPER(_DBG_CONCAT_HELPER(_DBG_APPLY_F,_DBG_COUNT_ARGS(__VA_ARGS__)),f,__VA_ARGS__)")
