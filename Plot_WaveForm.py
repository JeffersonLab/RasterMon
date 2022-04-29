#!/usr/bin/env python
import sys
sys.path.append("/Users/maurik/root/root.git/lib")
import ROOT
import numpy as np

try:
    import plotly.graph_objects as go
    from plotly.subplots import make_subplots
    import plotly.io as pio
    import chart_studio.plotly as charts

    pio.renderers.default = "browser"
    pio.templates.default = "plotly_white"

except ImportError:
    print("Sorry, but to make the nice plots, you really need a computer with 'plotly' installed.")
    sys.exit(1)

ROOT.gSystem.Load("cbuild/libRasterLib.dylib")

if len(sys.argv) < 2:
    print(f"Please specify input file. {sys.argv[0]}")
    sys.exit(1)

ras = ROOT.RasterEvioTool(sys.argv[1])
ras.Next()
ras.Next()
print(f"Analyzing the Raster data for run: {ras.GetRunNumber()} (this is zero for special runs.)")
#
if len(sys.argv) > 2:
    N = int(sys.argv[2])
else:
    N = 70000

if len(sys.argv) > 3:
    skip = int(sys.argv[3])
else:
    skip = 0

for i in range(skip):
    ras.Next()

times = np.zeros(N)
evtnum = np.zeros(N)
adc0 = np.zeros(N)
adc1 = np.zeros(N)
hover = [""]*N
i = 0
first_time = ras.GetTimeRasterCrate()
while i < N:
    ras.Next()
    evtnum[i] = ras.GetEventNumberRasterCrate()
    times[i] = 5*(ras.GetTimeRasterCrate() - first_time)/1E9
    hover[i] = f"time: {times[i]:6.3f}s<br />event: {i+skip:10,d}"
    adc0[i] = ras.GetRaster(0)
    adc1[i] = ras.GetRaster(1)
    i += 1

fig = make_subplots(rows=2, cols=3, specs=[[{}, {}, {}], [{"colspan": 3}, None, None]],
                    subplot_titles=["adc1 vs adc0, first 1/2 of cycle.", "adc1 vs adc0, first 1/2 of cycle.",
                                    "adc1 vs adc0 histogram", "adc values versus time stamp."])
# fig.add_trace(go.Scatter(y=evtnum, name="event number", line=dict(color="green")), row=1, col=1, secondary_y=True)
# fig.add_trace(go.Scatter(y=times, name="time", line=dict(color="blue")), row=1, col=1)
fig.add_trace(go.Scatter(x=adc0[0:N//2], y=adc1[0:N//2], name="adc1 vs adc0",
                         mode='markers',
                         marker_size=1.5,
                         hovertext=hover[0:N//2],
                         marker_color=times[0:N//2]), row=1, col=1)
fig.add_trace(go.Scatter(x=adc0[N//2:N], y=adc1[N//2:N], name="adc1 vs adc0",
                          mode='markers',
                          marker_size=1.5,
                         hovertext=hover[N//2:N],
                          marker_color=times[N//2:N]), row=1, col=2)

# fig.add_trace(go.Scatter(x=adc0, y=adc1, name="adc1 vs adc0",
#                          mode='markers',
#                          hovertext=hover,
#                          marker_size=1.5,
#                          marker_color=times), row=1, col=3)

fig.add_trace(go.Histogram2d(x=adc0, y=adc1,
               autobinx=False,
               # histnorm='probability',
               xbins=dict(start=0., end=4096, size=20.),
               autobiny=False,
               ybins=dict(start=0., end=4096., size=20.),
              # colorscale=[[0, 'rgb(12,51,131)'], [0.25, 'rgb(10,136,186)'], [0.5, 'rgb(242,211,56)'], [0.75, 'rgb(242,143,56)'], [1, 'rgb(217,30,30)']]
               ),
              row=1, col=3)


fig.add_trace(go.Scatter(x=times, y=adc0, name="adc0", hovertext=hover, line=dict(color="red")), row=2, col=1)
fig.add_trace(go.Scatter(x=times, y=adc1, name="adc0", hovertext=hover, line=dict(color="green")), row=2, col=1)
fig.update_layout(
    title=go.layout.Title(
        text="Plots Raster ADC Bank 59",
        yanchor="top",
        y=0.98,
        xanchor="center",
        x=0.5,
        font=dict(size=24)))
fig.update_xaxes(title_text="ADC 0", row=1, col=1)
fig.update_yaxes(title_text="ADC 1", row=1, col=1)
fig.update_xaxes(title_text="ADC 0", row=1, col=2)
fig.update_yaxes(title_text="ADC 1", row=1, col=2)
fig.update_xaxes(title_text="ADC 0", row=1, col=3)
fig.update_yaxes(title_text="ADC 1", row=1, col=3)

fig.update_xaxes(title_text="5* time stamp/1e9 [s]", row=2, col=1)
fig.update_yaxes(title_text="ADC raw value", row=2, col=1)

fig.write_html("raster_adc.html")
fig.write_image("raster_adc.pdf", width=4000, height=2000)
fig.show()
