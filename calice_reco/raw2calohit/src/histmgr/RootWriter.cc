#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TDirectory.h>
#include <TFile.h>
#include <new>
#include <iostream>
#include <cassert>

#include "RootWriter.hh"
namespace histmgr {
RootWriter::RootWriter(const std::string &file_name)
{
  _file=new TFile(file_name.c_str(),"UPDATE");
  
  if (!_file || !_file->IsWritable()) {
    delete _file;
    _file=new TFile(file_name.c_str(),"CREATE");
  }
  if (!_file) throw std::bad_alloc();
  _dirStack.push_back(_file);
}

RootWriter::~RootWriter() 
{
  delete _file;
}
  
bool RootWriter::enterDir(const std::string &name, bool create)
{

  _dirStack.back()->cd();
  TDirectory *a_dir=enterDir(_dirStack.back(),name);
  
  if (!a_dir && create) {
    _dirStack.back()->mkdir(name.c_str());
    a_dir=enterDir(_dirStack.back(),name);
    if (!a_dir) {
      std::cerr << "RootWriter::enterDir> Could not create directory" << name << std::endl;
    }
  }
  if (a_dir) {
    _dirStack.push_back(a_dir);
    a_dir->cd();    
    return true;
  }
  else {
    return false;
  }
}

void RootWriter::writeToCurrentDir(const FloatHistogram1D &hist, const std::string &name)
{
  _dirStack.back()->cd();

  TH1 *root_hist=createTH1(hist,name);
  root_hist->Write();
  delete root_hist;
}

void RootWriter::writeToCurrentDir(const FloatHistogram2D &hist, const std::string &name)
{
  _dirStack.back()->cd();

  TH2 *root_hist=createTH2(hist,name);
  root_hist->Write();
  delete root_hist;
}

void RootWriter::writeToCurrentDir(const EVENT::LCGenericObject *array_x, const EVENT::LCGenericObject *array_y, const EVENT::LCGenericObject *array_ey, const std::string &name)
{
  _dirStack.back()->cd();

  TGraph *root_graph=0;
  if (array_ey) {
    root_graph=createGraph(array_x,array_y, array_ey,name);
  }
  else {
    root_graph=createGraph(array_x,array_y,name);
  }
  root_graph->Write();
  delete root_graph;
}

 
void RootWriter::upDir()
{
  if (_dirStack.size()>1) 
    _dirStack.pop_back();
}
 
TDirectory *RootWriter::enterDir(TDirectory *dir, const std::string &name)
{
  if (dir && !name.empty()) {
    TObject *obj=dir->Get(name.c_str());
    if (obj) {
      if (obj->InheritsFrom(TDirectory::Class())) {
	TDirectory *a_dir=(TDirectory *) obj;
	a_dir->cd();
	return a_dir;
      }
    }
  }
  return 0;
}

TH1 *RootWriter::createTH1(const FloatHistogram1D &src_hist, const std::string &name)
{
  TH1 *dest_hist=new TH1F(name.c_str(),name.c_str(),src_hist.nBins(),src_hist.xMin(),src_hist.xMax());
  if (!dest_hist) return 0;

  dest_hist->SetDirectory(0);
  dest_hist->SetBinContent(0,src_hist.underflow());
  dest_hist->SetBinContent(dest_hist->GetNbinsX()+1,src_hist.overflow());
  UInt_t dest_bin_i=1;
  for (UInt_t bin_i=src_hist.firstBinIndex(); bin_i<=src_hist.lastBinIndex(); bin_i++) {
    dest_hist->SetBinContent(dest_bin_i,src_hist.binContent(bin_i));
    dest_bin_i++;
  }
  dest_hist->SetEntries(src_hist.entries());
  return dest_hist;
}

TH2 *RootWriter::createTH2(const FloatHistogram2D &src_hist, const std::string &name)
{
  TH2 *dest_hist=new TH2F(name.c_str(),name.c_str(),src_hist.xNBins(),src_hist.xMin(),src_hist.xMax(),src_hist.yNBins(),src_hist.yMin(),src_hist.yMax());
  if (!dest_hist) return 0;

  dest_hist->SetDirectory(0);
  assert ( src_hist.xUnderflowBinIndex() == 0) ;
  assert ( src_hist.xOverflowBinIndex() == src_hist.xNBins()+1 ) ;
  assert ( src_hist.yUnderflowBinIndex() == 0) ;
  assert ( src_hist.yOverflowBinIndex() == src_hist.yNBins()+1 ) ;

  for (UInt_t binx_i=src_hist.xUnderflowBinIndex(); binx_i<=src_hist.xOverflowBinIndex(); binx_i++) {
    for (UInt_t biny_i=src_hist.yUnderflowBinIndex(); biny_i<=src_hist.yOverflowBinIndex(); biny_i++) {
      UInt_t dest_bin_i=dest_hist->GetBin(binx_i,biny_i);
      UInt_t src_bin_i=src_hist.binIndex(binx_i,biny_i);
      dest_hist->SetBinContent(dest_bin_i,src_hist.binContent(src_bin_i));
    }
  }
  dest_hist->SetEntries(src_hist.entries());
  return dest_hist;
}

TGraph *RootWriter::createGraph(const EVENT::LCGenericObject *array_x, const EVENT::LCGenericObject *array_y, const std::string &name) 
{
  assert(array_x->getNDouble() == array_y->getNFloat() );
  TGraph *a_graph=new TGraph;
  if (!a_graph) return 0;

  a_graph->Set(array_x->getNDouble());
  for (unsigned int i=0; i<static_cast<unsigned int>(array_x->getNDouble()); i++) {
    a_graph->SetPoint(i,array_x->getDoubleVal(i),array_y->getFloatVal(i));
  }
  a_graph->SetName(name.c_str());
  return a_graph;
}

TGraph *RootWriter::createGraph(const EVENT::LCGenericObject *array_x, const EVENT::LCGenericObject *array_y, const EVENT::LCGenericObject *array_ey, const std::string &name)
{
  assert(array_x->getNDouble() == array_y->getNFloat() );
  assert(array_x->getNDouble() == array_ey->getNFloat() );

  TGraphErrors *a_graph=new TGraphErrors;
  if (!a_graph) return 0;

  a_graph->Set(array_x->getNDouble());

  for (unsigned int i=0; i<static_cast<unsigned int>(array_x->getNDouble()); i++) {
    a_graph->SetPoint(i,array_x->getDoubleVal(i),array_y->getFloatVal(i));
    a_graph->SetPointError(i,0,array_ey->getFloatVal(i));
  }
  a_graph->SetName(name.c_str());
  return a_graph;
}

}

