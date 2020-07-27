#!/usr/bin/env python
# -*- coding: utf-8 -*-

if __name__ == "__main__":

  path_r = "script_radio_control.html"
  path_w = "script_radio_control_test.csv"

  with open(path_r, mode='r') as rf:
    with open(path_w, mode='w') as wf:
      for s_line in rf:
        work = s_line.replace('\"', '\'')
        work = work.replace('\n', '')
        wf.writelines('\"' + work + '\\r\\n\"\n')

