//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Implementation of PXI graphing canvas window
//

#ifndef PXICANVAS_H
#define PXICANVAS_H

#include "pxi.h"
#include "pxiconf.h"

class PxiCanvas : public wxScrolledWindow {
friend class PxiChild;
public:
  typedef struct LABELSTRUCT {
    friend gOutput &operator<<(gOutput &op,const LABELSTRUCT &l);
    wxString label;
    double x,y;
    LABELSTRUCT(void):label(""),x(0.0),y(0.0) {}
    LABELSTRUCT &operator=(const LABELSTRUCT &l)
      {label=l.label;x=l.x;y=l.y; return *this;}
    int operator==(const LABELSTRUCT &l)
      {return (label==l.label && x==l.x && y==l.y);}
    int operator!=(const LABELSTRUCT &l)
      {return !(*this==l);}
  } label_struct;

private:
  PxiDrawSettings *draw_settings;       // draw settings, see above

  ExpData *exp_data;                    // experimental data overlay
  gBlock<label_struct> labels;          // labels for generic text
  DataLine probs;                       // a line of data
  gBlock<FileHeader> headers;           // all the basic info about the file
  Bool	stopped;
  double cur_e;
  bool painting;
  bool m_landscape;                     // landscap mode if true
  int m_width, m_height;                // width, height of page
  double m_scale;                        // scaling factor
  double m_ppu;                        // pixels per scroll unit

  int Width(void) const 
    {if(m_landscape) return m_height; return m_width;}
  int Height(void) const 
    {if(m_landscape) return m_width; return m_height;}
  void SetLandscape(bool flag) 
    {m_landscape = flag;}
  bool GetLandscape(void) 
    {return m_landscape;}
  double GetScale(void) const {return m_scale;} 
  void SetScale(double x);

  void DoPlot_X(wxDC& dc,const PlotInfo &thisplot,
		int x0, int y0, int cw,int ch, int level=1);
  void DoPlot_2(wxDC& dc,const PlotInfo &thisplot,
		int x0, int y0, int cw,int ch, int level=1);
  void DoPlot_3(wxDC& dc,const PlotInfo &thisplot,
		int x0, int y0, int cw,int ch, int level=1);

  void PlotData_X(wxDC& dc, const PlotInfo &thisplot, 
		  int x0, int y0, int cw,int ch,
		  const FileHeader &f_header, int level);
  void PlotData_2(wxDC& dc, const PlotInfo &thisplot, 
		  int x0, int y0, int cw,int ch,
		  const FileHeader &f_header, int level=1);
  void PlotData_3(wxDC& dc, const PlotInfo &thisplot, 
		  int x0, int y0, int cw,int ch,
		  const FileHeader &f_header, int level=1);

  void DrawExpPoint_X(wxDC &dc, const PlotInfo &thisplot, 
		      double cur_e,int iset,int st,
		      int x0, int y0, int ch,int cw);
  void DrawExpPoint_2(wxDC &dc, const PlotInfo &thisplot, 
		      double cur_e,int pl1,int st1,
		      int pl2,int st2,int x0, int y0, int cw, int ch);
  
  void DrawExpPoint_3(wxDC &dc,const PlotInfo &thisplot, 
		      double cur_e,int iset,int st1,int st2,
		      int x0, int y0, int cw,int ch);

  void PlotLabels(wxDC &dc,int ch,int cw);


  double CalcY_X(double y,int x0, int ch, const PlotInfo &thisplot);
  double CalcX_X(double x,int y0, int cw, const PlotInfo &thisplot);
  double CalcY_3(double p1,int x0, int y0, int ch,int cw);
  double CalcX_3(double p1,double p2,int x0, int y0, int ch,int cw, const PlotInfo &thisplo);
  
  void DrawToken(wxDC &dc, int x, int y, int st);

public:
  PxiCanvas(wxFrame *frame, const wxPoint &p_position,
	    const wxSize &p_size, int style=0,const char *file_name=NULL);
  void Update(wxDC& dc,int device);
  void OnPaint(wxPaintEvent &ev);
  void OnChar(wxKeyEvent &ev);
  void OnEvent(wxMouseEvent &ev);
  void ShowDetail(void);
  void StopIt(void);
  void Render(void) {wxClientDC dc(this); dc.BeginDrawing();Render(dc);dc.EndDrawing();}
  void Render(wxDC &dc) {Update(dc,PXI_UPDATE_SCREEN);}
  void SetPage(int page) {draw_settings->SetPage(page);}
  void SetNextPage(void) {draw_settings->SetNextPage();}
  void SetPreviousPage(void) {draw_settings->SetPreviousPage();}

  PxiDrawSettings *DrawSettings(void) {return draw_settings;}
  void NewExpData(ExpDataParams &P);
  wxString PxiName(void) const {return headers[1].FileName();}
  const FileHeader &Header(int i) {return headers[i];}
  void  AppendHeader(const FileHeader &h) {headers.Append(h);}

  DECLARE_EVENT_TABLE()
};

#endif  // PXICANVAS_H