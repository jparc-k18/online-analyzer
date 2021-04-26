#!/usr/bin/env python3

import logging
import os
import sys
import time
import ROOT

spill_length = 48406790
error_length = 100

#scaler_dir = os.path.abspath('/data3/E42SubData/scaler_2021may')
scaler_dir = os.path.abspath('/misc/scaler_e42_2021may')
scaler_list = [os.path.join(scaler_dir, 'hul01scr-1.txt'),
               os.path.join(scaler_dir, 'hul01scr-2.txt')]
output_dir = '/data3/E42SubData/scaler_2021may'

channel_map = {
  # 'name': [hul, channel]
  'BH1': [0, 16],
  'BH1-SUM': [-1, -1],
  'BH1-01': [1, 0],
  'BH1-02': [1, 1],
  'BH1-03': [1, 2],
  'BH1-04': [1, 3],
  'BH1-05': [1, 4],
  'BH1-06': [1, 5],
  'BH1-07': [1, 6],
  'BH1-08': [1, 7],
  'BH1-09': [1, 8],
  'BH1-10': [1, 9],
  'BH1-11': [1, 10],
  'BH2': [0, 17],
  'BH2-SUM': [-1, -1],
  'BH2-01': [0, 64],
  'BH2-02': [0, 65],
  'BH2-03': [0, 66],
  'BH2-04': [0, 67],
  'BH2-05': [0, 68],
  'BH2-06': [0, 69],
  'BH2-07': [0, 70],
  'BH2-08': [0, 71],
  'BAC': [0, 18],
  'HTOF': [0, 20],
  'SCH': [0, 11],
  'TOF': [0, 22],
  '10M-Clock': [0, 0],
  'BH1xBH2': [0, 39],
  'K-Beam': [0, 39],
  'Pi-Beam': [0, 40],
  'BH1-1/100-PS': [1, 11],
  'BH1-1/1e5-PS': [1, 12],
  'TOF-24': [0, 29],
  'Mtx2D-1': [0, 32],
  'Mtx2D-2': [0, 33],
  'Mtx3D': [0, 34],
  'Other1': [0, 25],
  'Other2': [0, 26],
  'Other3': [0, 27],
  'Other4': [0, 28],
  'BEAM-A': [0, 35],
  'BEAM-B': [0, 36],
  'BEAM-C': [0, 37],
  'BEAM-D': [0, 38],
  'BEAM-E': [0, 39],
  'BEAM-F': [0, 40],
  'HTOF-Mp2': [0, 20],
  'HTOF-Mp3': [0, 21],
  'HTOF-Mp4': [0, -1],
  'HTOF-Mp5': [0, -1],
  'BVH': [0, -1],
  'LAC': [0, 23],
  'WC': [0, 24],
  'Spill': [-1, -1],
  'TM': [0, 9],
  'SY': [0, 10],
  'Real-Time': [0, 1],
  'Live-Time': [0, 2],
  'L1-Req': [0, 3],
  'L1-Acc': [0, 4],
  # 'MstClr': [0, 5],
  'Clear': [0, 6],
  'L2-Req': [0, 7],
  'L2-Acc': [0, 8],
  'TRIG-A': [0, 41],
  'TRIG-B': [0, 42],
  'TRIG-C': [0, 43],
  'TRIG-D': [0, 44],
  'TRIG-E': [0, 45],
  'TRIG-F': [0, 46],
  'TRIG-A-PS': [0, 48],
  'TRIG-B-PS': [0, 49],
  'TRIG-C-PS': [0, 50],
  'TRIG-D-PS': [0, 51],
  'TRIG-E-PS': [0, 52],
  'TRIG-F-PS': [0, 53],
  'TRIG-PSOR-A': [0, 54],
  'TRIG-PSOR-B': [0, 55],
  'Clock-PS': [0, 56],
  'Reserve2-PS': [0, 57],
  'Level1-PS': [0, 58],
}

inverse_map = { str(v): k for k, v in channel_map.items() }

#______________________________________________________________________________
def separate_comma(value):
  value = int(value)
  sep_num = list()
  while int(value/1000) > 0:
    sep_num.append(value%1000)
    value = int(value/1000)
  ret = str(value)
  for itr in reversed(sep_num):
    ret += f',{itr:03d}'
  return ret

#______________________________________________________________________________
class ScalerAnalyzer():

  #____________________________________________________________________________
  def __init__(self, port=9090):
    ROOT.gROOT.SetBatch()
    ROOT.gErrorIgnoreLevel = ROOT.kFatal
    ROOT.gSystem.Setenv('SCALER_PRINT', '0')
    self.__port = port
    self.__server = ROOT.THttpServer(f'http:{self.__port}?loopback?thrds=5')
    self.__server.Restrict('/', 'allow=all')
    self.__server.CreateItem('/', 'Online Scaler')
    self.__server.SetJSROOT('https://root.cern.ch/js/latest/')
    self.__server.RegisterCommand('/Print10Spill',
                                  'gSystem->Setenv("SCALER_PRINT", "1")')
    self.__server.SetItemField('/','_monitoring','100')
    self.__server.SetItemField('/','_layout','vert2')
    self.__server.SetItemField('/','_drawitem','[Spill,Canvases/Trend]')
    # self.__server.SetItemField('/','_drawitem','Scaler')
    self.__server.CreateItem('/Spill', 'Online Scaler')
    self.__server.SetItemField('/Spill', '_kind', 'Text')
    self.__server.CreateItem('/Run', 'Online Scaler')
    self.__server.SetItemField('/Run', '_kind', 'Text')
    # self.__server.CreateItem('/Tag', 'Tag Check')
    # self.__server.SetItemField('/Tag', '_kind', 'Text')
    # content = ('<div style="color: white; background-color: black;' +
    #            'width: 100%; height: 100%;">' +
    #            'Tag cheker is running' + '</div>')
    # self.__server.SetItemField('/Tag', 'value', content)
    self.__now = ROOT.TTimeStamp()
    self.__now.Add(-self.__now.GetZoneOffset())
    self.__spill_end = False
    self.__ignore = False
    self.__runno = 0
    self.__evno = 0
    self.__prev_runno = 0
    self.__prev_evno = 0
    self.__scaler = dict()
    self.__scaler_runsum = dict()
    for k, v in channel_map.items():
      self.__scaler[k] = 0
      self.__scaler_runsum[k] = 0
    self.__footer = self.__calculate_footer(self.__scaler)
    self.__footer_runsum = self.__calculate_footer(self.__scaler_runsum)
    self.__nraw = 27
    self.__runnos = [-1, -2]
    self.__evnos = [-1, -2]
    ''' graph '''
    self.__nplot = 200
    self.__plot_unixtime = list()
    for i in range(self.__nplot):
      self.__plot_unixtime.append(self.__now.GetSec() + 5.2*(i - self.__nplot))
    self.__graph_beam = list()
    self.__plot_beam = list()
    canvas_trend = ROOT.TCanvas('Trend', 'Trend')
    canvas_trend.Divide(2, 1)
    self.__server.Register('/', canvas_trend)
    self.__server.Hide('/Trend')
    canvas_trend.cd(1).SetGridy()
    self.__legend_beam = ROOT.TLegend(0.130, 0.130, 0.280, 0.410)
    self.__legend_beam.SetTextSize(0.05)
    self.__legend_beam.SetFillColor(0)
    self.__legend_beam.SetBorderSize(4)
    for i, l in enumerate(['K-Beam', '#pi-Beam', 'BH1xBH2']):
      self.__plot_beam.append([0 for i in range(self.__nplot)])
      graph = ROOT.TGraph()
      graph.SetName(l)
      graph.SetTitle('Beam')
      graph.SetMarkerStyle(8)
      graph.SetMarkerSize(1.5)
      graph.SetMarkerColor([ROOT.kGreen+1, ROOT.kBlue+1, ROOT.kBlack][i])
      graph.SetLineWidth(3)
      graph.SetLineColor([ROOT.kGreen+1, ROOT.kBlue+1, ROOT.kBlack][i])
      graph.GetXaxis().SetTimeDisplay(1)
      graph.GetXaxis().SetTimeFormat('%m/%d %H:%M')
      # graph.GetXaxis().SetLabelOffset(0.04)
      graph.GetXaxis().SetTimeOffset(0, 'jpn')
      graph.SetPoint(0, 0, 0)
      if i == 0:
        graph.Draw('AL')
      else:
        graph.Draw('L')
      self.__graph_beam.append(graph)
      self.__legend_beam.AddEntry(graph, l, 'P')
    self.__legend_beam.SetHeader(f'K/#pi Ratio : {0:5.3f}')
    self.__legend_beam.Draw()
    self.__graph_daq = list()
    self.__plot_daq = list()
    canvas_trend.cd(2).SetGridy()
    self.__legend_daq = ROOT.TLegend(0.130, 0.130, 0.280, 0.410)
    self.__legend_daq.SetTextSize(0.05)
    self.__legend_daq.SetFillColor(0)
    self.__legend_daq.SetBorderSize(4)
    for i, l in enumerate(['DAQ-Eff', 'L2-Eff', 'Duty']):
      self.__plot_daq.append([0 for i in range(self.__nplot)])
      graph = ROOT.TGraph()
      graph.SetName(l)
      graph.SetTitle('DAQ')
      graph.SetMarkerStyle(8)
      graph.SetMarkerSize(1.5)
      graph.SetMarkerColor([ROOT.kRed+1, ROOT.kOrange+1, ROOT.kBlue+1][i])
      graph.SetLineWidth(3)
      graph.SetLineColor([ROOT.kRed+1, ROOT.kOrange+1, ROOT.kBlue+1][i])
      graph.GetXaxis().SetTimeDisplay(1)
      graph.GetXaxis().SetTimeFormat('%m/%d %H:%M')
      # graph.GetXaxis().SetLabelOffset(0.04)
      graph.GetXaxis().SetTimeOffset(0, 'jpn')
      graph.SetPoint(0, 0, 0)
      if i == 0:
        graph.Draw('AL')
      else:
        graph.Draw('L')
      self.__graph_daq.append(graph)
      self.__legend_daq.AddEntry(graph, l, 'P')
    self.__legend_daq.SetHeader(f'DAQ Eff. : {0:5.3f}')
    self.__legend_daq.Draw()

 #____________________________________________________________________________
  def __calculate_footer(self, scaler_dict):
    ret = dict()
    ret['BH2/TM'] = (
      scaler_dict['BH2']/scaler_dict['TM'] if scaler_dict['TM'] > 0
      else ROOT.TMath.QuietNaN())
    ret['Live/Real'] = (
      scaler_dict['Live-Time']/scaler_dict['Real-Time']
      if scaler_dict['Real-Time'] > 0
      else ROOT.TMath.QuietNaN())
    ret['DAQ-Eff'] = (
      scaler_dict['L1-Acc']/scaler_dict['L1-Req']
      if scaler_dict['L1-Req'] > 0
      else ROOT.TMath.QuietNaN())
    ret['L1Req/BH2'] = (
      scaler_dict['L1-Req']/scaler_dict['BH2']
      if scaler_dict['BH2'] > 0
      else ROOT.TMath.QuietNaN())
    ret['L2-Eff'] = (
      scaler_dict['L2-Acc']/scaler_dict['L1-Acc']
      if scaler_dict['L1-Acc'] > 0
      else ROOT.TMath.QuietNaN())
    ret['Duty-Factor'] = min(
      ret['DAQ-Eff'] / (1. - ret['DAQ-Eff']) *
      (1./ret['Live/Real'] - 1)
      if (ret['Live/Real'] > 0 and ret['DAQ-Eff'] < 1.)
      else 1., 1.)
    return ret

  #____________________________________________________________________________
  def __draw_one_box(self, x, y, title, val):
    if title == 'Pi-Beam':
      title = '#pi-Beam'
    tex = ROOT.TLatex()
    tex.SetNDC()
    tex.SetTextSize(0.04)
    tex.SetTextAlign(12)
    tex.DrawLatex(x, y, title)
    tex.SetTextAlign(32)
    if 0.1 < x and x < 0.5:
      tex.DrawLatex(x + 0.295, y, val)
    else:
      tex.DrawLatex(x + 0.280, y, val)

  #____________________________________________________________________________
  def __draw_one_line(self, line_no, info_array, footer=False):
    ystep = 0.05
    y0 = 0.95
    if len(info_array) == 3 and footer:
      self.__draw_one_line(line_no,
                           [info_array[0], f'{self.__footer[info_array[0]]:.6f}',
                            info_array[1], f'{self.__footer[info_array[1]]:.6f}',
                            info_array[2], f'{self.__footer[info_array[2]]:.6f}'])
    elif len(info_array) == 3 and not footer:
      self.__draw_one_line(line_no, [info_array[0], self.get(info_array[0]),
                                     info_array[1], self.get(info_array[1]),
                                     info_array[2], self.get(info_array[2])])
    elif len(info_array) == 6:
      y = y0 - ystep*line_no
      x = [0.05, 0.35, 0.67]
      for i in range(3):
        self.__draw_one_box(x[i], y, info_array[2*i], info_array[2*i + 1])
      line = ROOT.TLine()
      line.SetNDC()
      line.SetLineColor(ROOT.kGray)
      line.DrawLine(0.05, y-0.5*ystep, 0.95, y-0.5*ystep)
      line.SetLineColor(ROOT.kBlack)
      line.DrawLine(0.34, y-0.5*ystep, 0.34, y+0.5*ystep)
      line.DrawLine(0.66, y-0.5*ystep, 0.66, y+0.5*ystep)
      if (line_no == 1 or line_no == 5 or line_no ==14):
        line.DrawLine(0.05, y-0.5*ystep, 0.95, y-0.5*ystep)

  #____________________________________________________________________________
  def __print_scaler_sheet(self, file_name):
    c1 = ROOT.TCanvas('c1', 'c1', 1200, 800)
    self.__draw_one_line(1, [self.__now.AsString('s'), '',
                             'Event#', separate_comma(self.__evno),
                             'Run#', separate_comma(self.__runno)])
    self.__draw_one_line(2, ['Spill', 'TM', '10M-Clock'])
    self.__draw_one_line(3, ['BH1xBH2', 'SY', 'BH1-1/100-PS'])
    self.__draw_one_line(4, ['K-Beam', 'BH1-SUM', 'BH1-1/1e5-PS'])
    self.__draw_one_line(5, ['Pi-Beam', 'BH2-SUM', 'TOF-24'])
    self.__draw_one_line(6, ['BH1', 'TRIG-A-PS', 'L1-Req'])
    self.__draw_one_line(7, ['BH2', 'TRIG-B-PS', 'L1-Acc'])
    self.__draw_one_line(8, ['BAC', 'TRIG-C-PS', 'Clear'])
    self.__draw_one_line(9, ['HTOF', 'TRIG-D-PS', 'L2-Req'])
    self.__draw_one_line(10, ['SCH', 'TRIG-E-PS', 'L2-Acc'])
    self.__draw_one_line(11, ['TOF', 'TRIG-F-PS', 'HTOF-Mp2'])
    self.__draw_one_line(12, ['BVH', 'Mtx2D-1', 'HTOF-Mp3'])
    self.__draw_one_line(13, ['LAC', 'Mtx2D-2', 'HTOF-Mp4'])
    self.__draw_one_line(14, ['WC', 'Mtx3D', 'HTOF-Mp5'])
    self.__draw_one_line(15, ['BH2/TM', 'Live/Real', 'DAQ-Eff'],
                         footer=True)
    self.__draw_one_line(16, ['L1Req/BH2', 'L2-Eff', 'Duty-Factor'],
                         footer=True)
    output_path = os.path.join(output_dir, file_name)
    c1.Print(output_path)
    logging.info(f'print {output_path}')

  #____________________________________________________________________________
  def get(self, key):
    return separate_comma(self.__scaler[key]) if key in self.__scaler else ''

  #____________________________________________________________________________
  def make_summary(self, file_name, scaler_list, footer_list):
    output_path = os.path.join(output_dir, file_name)
    buf = str()
    with open(output_path, 'w') as f:
      buf += ('# Generated by ' + os.path.abspath(sys.argv[0]) +
              ' on ' + self.__now.AsString('s') + '\n\n')
      buf += f'{"Run":<20}{self.__prev_runno:>20}\n'
      buf += f'{"Event":<20}{self.__prev_evno:>20}\n'
      for key, val in scaler_list.items():
        if key == 'n/a':
          continue
        buf += f'{key:<20}{val:>20}\n'
      buf += '\n'
      for key, val in footer_list.items():
        buf += f'{key:<20}{val:>20.9f}\n'
      f.write(buf)

  #____________________________________________________________________________
  def run(self):
    while not ROOT.gSystem.ProcessEvents():
      self.update_scaler()
      self.update_content('/Spill', self.__scaler, self.__footer)
      self.update_content('/Run', self.__scaler_runsum, self.__footer_runsum)
      if self.__spill_end:
        self.make_summary('spill_end.txt', self.__scaler, self.__footer)
        self.update_graph()
      time.sleep(0.1)

  #____________________________________________________________________________
  def update_content(self, label, scaler_dict, footer_dict):
    if self.__ignore:
      return
    i = 0
    disp_left = list()
    disp_center = list()
    disp_right = list()
    disp_left.append(['RUN', separate_comma(self.__runno)])
    disp_center.append(['Event Number', separate_comma(self.__evno)])
    disp_right.append(['', 'Spill End' if self.__spill_end else ''])
    disp_left.append(['', ''])
    disp_center.append(['', ''])
    print_flag = ROOT.gSystem.Getenv('SCALER_PRINT')
    disp_right.append(['Printing ...' if print_flag == '1' else '', ''])
    for k, v in scaler_dict.items():
      if i < self.__nraw:
        disp_left.append([k, separate_comma(v)])
      elif self.__nraw <= i and i < self.__nraw*2:
        disp_center.append([k, separate_comma(v)])
      elif self.__nraw*2 <= i and i < self.__nraw*3:
        disp_right.append([k, separate_comma(v)])
      i += 1
    content = ('<div style="color: white; background-color: black;' +
               'width: 100%; height: 100%;">' +
               '<table border="0" width="700" cellpadding="0">')
    for i in range(self.__nraw+2):
      if i == len(disp_left):
        disp_left.append(['n/a', '0'])
      if i == len(disp_center):
        disp_center.append(['n/a', '0'])
      if i == len(disp_right):
        disp_right.append(['n/a', '0'])
      content += '<tr>'
      content += (f'<td width="100">{disp_left[i][0]}</td>' +
                  f'<td align="right" width="100">{disp_left[i][1]}</td>')
      content += (f'<td width="100"> : {disp_center[i][0]}</td>' +
                  f'<td align="right" width="100">{disp_center[i][1]}</td>')
      content += (f'<td width="100"> : {disp_right[i][0]}</td>' +
                  f'<td align="right" width="100">{disp_right[i][1]}</td>')
      content += '</tr>'
    content += '<tr>'
    content += '<td width="100"></td><td align="right" width="100"></td>'
    content += '<td width="100"> : </td><td align="right" width="100"></td>'
    content += '<td width="100"> : </td><td align="right" width="100"></td>'
    content += '</tr>'
    content += '<tr>'
    content += ('<td width="100">BH2/TM</td>' +
                '<td align="right" width="100">' +
                f'{footer_dict["BH2/TM"]:.6f}</td>')
    content += ('<td width="100"> : Live/Real</td>' +
                '<td align="right" width="100">' +
                f'{footer_dict["Live/Real"]:.6f}</td>')
    content += ('<td width="100"> : DAQ-Eff</td>' +
                '<td align="right" width="100">' +
                f'{footer_dict["DAQ-Eff"]:.6f}</td>')
    content += '</tr>'
    content += '<tr>'
    content += ('<td width="100">L1Req/BH2</td>' +
                '<td align="right" width="100">' +
                f'{footer_dict["L1Req/BH2"]:.6f}</td>')
    content += ('<td width="100"> : L2-Eff</td>' +
                '<td align="right" width="100">' +
                f'{footer_dict["L2-Eff"]:.6f}</td>')
    content += ('<td width="100"> : Duty-Factor</td>' +
                '<td align="right" width="100">' +
                f'{footer_dict["Duty-Factor"]:.6f}</td>')
    content += '</tr>'
    content += '</table></div>'
    self.__server.SetItemField(label, 'value', content)

  #____________________________________________________________________________
  def update_graph(self):
    kpi = (self.__scaler['K-Beam']/self.__scaler['Pi-Beam']
           if self.__scaler['Pi-Beam'] > 0 else ROOT.TMath.QuietNaN())
    self.__legend_beam.SetHeader(f'K/#pi Ratio : {kpi:5.3f}')
    values = [self.__scaler['K-Beam'],
              self.__scaler['Pi-Beam'],
              self.__scaler['BH1xBH2']]
    self.__plot_unixtime.pop(0)
    self.__plot_unixtime.append(self.__now.GetSec())
    max_count = 1
    for i, g in enumerate(self.__graph_beam):
      self.__plot_beam[i].pop(0)
      self.__plot_beam[i].append(values[i])
      g.Set(0)
      for j in range(self.__nplot):
        g.SetPoint(j, self.__plot_unixtime[j], self.__plot_beam[i][j])
        max_count = max(self.__plot_beam[i][j], max_count)
      g.GetYaxis().SetRangeUser(0, max_count*1.1)
    self.__legend_daq.SetHeader(f'DAQ Eff : {self.__footer["DAQ-Eff"]:5.3f}')
    values = [self.__footer['DAQ-Eff'],
              self.__footer['L2-Eff'],
              self.__footer['Duty-Factor']]
    for i, g in enumerate(self.__graph_daq):
      self.__plot_daq[i].pop(0)
      self.__plot_daq[i].append(values[i])
      g.Set(0)
      for j in range(self.__nplot):
        g.SetPoint(j, self.__plot_unixtime[j], self.__plot_daq[i][j])
      g.GetYaxis().SetRangeUser(0, 1.01)

  #____________________________________________________________________________
  def update_scaler(self):
    self.__now = ROOT.TTimeStamp()
    self.__now.Add(-self.__now.GetZoneOffset())
    for i, scaler_txt in enumerate(scaler_list):
      with open(scaler_txt, 'r') as f:
        columns = f.readline().split()
        if len(columns) != 2:
          continue
        self.__runnos[i] = int(columns[0])
        self.__evnos[i] = int(columns[1])
        for line in f.readlines():
          columns = line.split()
          if len(columns) != 2:
            continue
          index = [i, int(columns[0])]
          if str(index) in inverse_map:
            self.__scaler[inverse_map[str(index)]] = int(columns[1])
    self.__runno = max(self.__runnos[0], self.__runnos[1])
    self.__evno = max(self.__evnos[0], self.__evnos[1])
    if self.__evno == self.__prev_evno:
      self.__spill_end = False
      self.__ignore = True
      return
    if self.__runno != self.__prev_runno:
      self.__scaler['Spill'] = 0
      self.make_summary(f'scaler_{self.__prev_runno:05d}.txt',
                        self.__scaler_runsum,
                        self.__footer_runsum)
      for k, v in channel_map.items():
        self.__scaler_runsum[k] = 0
    self.__spill_end = (
      self.__runnos[0] == self.__runnos[1] and
      self.__evnos[0] == self.__evnos[1] and
      abs(self.__scaler['10M-Clock'] - spill_length) < error_length)
    if self.__spill_end:
      self.__scaler['Spill'] += 1
      for k, v in channel_map.items():
        self.__scaler_runsum[k] += self.__scaler[k]
      self.__scaler_runsum['Spill'] = self.__scaler['Spill']
      self.__footer_runsum = self.__calculate_footer(self.__scaler_runsum)
    self.__footer = self.__calculate_footer(self.__scaler)
    if ROOT.gSystem.Getenv('SCALER_PRINT') == '1':
      self.__print_scaler_sheet('scaler_sheet_tmp.pdf')
      ROOT.gSystem.Setenv('SCALER_PRINT', '0')
    self.__prev_runno = self.__runno
    self.__prev_evno = self.__evno
    self.__ignore = False

#______________________________________________________________________________
if __name__ == '__main__':
  formatter = '%(asctime)s [%(levelname)s] %(message)s'
  logging.basicConfig(level=logging.DEBUG,
                      format=formatter)
  logging.info('start scaler')
  try:
    ana = ScalerAnalyzer(port=9092)
    ana.run()
  except KeyboardInterrupt:
    print()
    logging.info('catch SIGINT')
    logging.info('stop scaler')
    ROOT.gSystem.Exit(0)
