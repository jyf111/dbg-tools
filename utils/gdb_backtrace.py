import gdb

class RefinedBt(gdb.Command):
  def __init__ (self):
    super(RefinedBt, self).__init__ ("rbt", gdb.COMMAND_USER)

  def framecount():
    n = 0
    f = gdb.newest_frame()
    while f:
      n = n + 1
      f = f.older()
    return n

  def invoke(self, arg, from_tty):
    bt_output = gdb.execute('bt', to_string=True, from_tty=True)
    bt_lines = bt_output.splitlines()
    bt_lines = bt_lines[1:]
    for line in bt_lines:
      refined_line = line
      if len(refined_line) > 0:
        if refined_line[0] == '#':
          i = 1
          while refined_line[i].isdigit():
            i += 1
          refined_line = '#' + str(int(refined_line[1:i]) - 1) + refined_line[i:]
      print(refined_line)

RefinedBt()
