#ifndef MY_SAMPLE_READER_H
#define MY_SAMPLE_READER_H

#include <iostream>
#include <string>
#include <vector>
#include <exception>

#include "TChain.h"
#include "THStack.h"
#include "TH1D.h"

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"

class SampleReaderException: public std::runtime_error
{
public:
  enum class ExceptionType {Base, FileNotFound, MissingParamsInJSON, EmptySampleInfo, EmptyProcessInfo, EmptySampleReader};

  explicit SampleReaderException(const std::string message):
    std::runtime_error(""),
    msg_(message),
    type_(ExceptionType::Base)
  {
  }

  explicit SampleReaderException(const std::string message, ExceptionType type):
    std::runtime_error(""),
    msg_(message),
    type_(type)
  {
  }

  virtual ~SampleReaderException() throw() {}

  virtual const char* what() const throw()
  {
    return msg_.c_str();
  }

  virtual void* extra_info() throw()
  {
    return nullptr;
  }

  ExceptionType type() {return type_;}

private:
protected:
  std::string msg_;
  ExceptionType type_;
};

class MissingJSONParam: public SampleReaderException
{
public:
  explicit MissingJSONParam(const std::string message):
    SampleReaderException(message, ExceptionType::MissingParamsInJSON)
  {
  }

private:
protected:
};

class FileNotFound: public SampleReaderException
{
public:
  explicit FileNotFound(const std::string message):
    SampleReaderException(message, ExceptionType::FileNotFound)
  {
  }

private:
protected:
};

class EmptySampleInfo: public SampleReaderException
{
public:
  explicit EmptySampleInfo(const std::vector<std::string> filesNotFound):
    SampleReaderException("", ExceptionType::EmptySampleInfo),
    missingFiles_(filesNotFound)
  {
    msg_ = "";
    for(auto & file : missingFiles_)
      msg_ += "'" + file + "', ";
    msg_.pop_back();
    msg_.pop_back();
  }

  virtual void* extra_info() throw()
  {
    return &missingFiles_;
  }

private:
protected:
  std::vector<std::string> missingFiles_;
};

class EmptyProcessInfo: public SampleReaderException
{
public:
  explicit EmptyProcessInfo(const std::vector<std::string> filesNotFound):
    SampleReaderException("", ExceptionType::EmptyProcessInfo),
    missingFiles_(filesNotFound)
  {
    msg_ = "";
    for(auto & file : missingFiles_)
      msg_ += "'" + file + "', ";
    msg_.pop_back();
    msg_.pop_back();
  }

  virtual void* extra_info() throw()
  {
    return &missingFiles_;
  }

private:
protected:
  std::vector<std::string> missingFiles_;
};

class SampleInfo
{
public:
  SampleInfo() = delete;
  SampleInfo(nlohmann::json jsonInfo, std::string baseDir = "", std::string suffix = "");
  //~SampleInfo();

  std::vector<std::string> getAllFiles() const {return filePaths_;};

  double crossSection() const {return crossSection_;}
  double branchingRatio() const {return branchingRatio_;}
  std::string tag() const {return tag_;}
  int split() const {return split_;}

  // Iteration
  typedef typename std::vector<std::string>::iterator iterator;
  typedef typename std::vector<std::string>::const_iterator const_iterator;

  iterator begin() {return filePaths_.begin();}
  const_iterator begin() const {return filePaths_.begin();}
  const_iterator cbegin() const {return filePaths_.cbegin();}
  iterator end() {return filePaths_.end();}
  const_iterator end() const {return filePaths_.end();}
  const_iterator cend() const {return filePaths_.cend();}

private:
  bool fileExists(std::string fileName);

protected:
  std::string baseDir_;
  std::string suffix_;
  double crossSection_;
  double branchingRatio_;
  std::string tag_;
  int split_;

  std::vector<std::string> filePaths_;
  std::vector<std::string> missingFiles_;
};

class ProcessInfo
{
public:
  ProcessInfo() = delete;
  ProcessInfo(nlohmann::json jsonInfo, std::string baseDir = "", std::string suffix = "");
  //~ProcessInfo();

  std::vector<std::string> getAllFiles();
  TH1D* getHist(std::string, std::string, std::string, int, double, double);
  doubleUnc getYield(std::string, std::string);

  std::string tag() const {return tag_;}
  std::string label() const {return label_;}
  bool isdata() const {return isdata_;}
  bool issignal() const {return issignal_;}
  bool isfastsim() const {return isfastsim_;}
  bool spimpose() const {return spimpose_;}
  int color() const {return color_;}
  int lcolor() const {return lcolor_;}
  int lwidth() const {return lwidth_;}
  int lstyle() const {return lstyle_;}
  int fill() const {return fill_;}
  int mcolor() const {return mcolor_;}
  int marker() const {return marker_;}

  // Iteration
  typedef typename std::vector<SampleInfo>::iterator iterator;
  typedef typename std::vector<SampleInfo>::const_iterator const_iterator;

  iterator begin() {return samples_.begin();}
  const_iterator begin() const {return samples_.begin();}
  const_iterator cbegin() const {return samples_.cbegin();}
  iterator end() {return samples_.end();}
  const_iterator end() const {return samples_.end();}
  const_iterator cend() const {return samples_.cend();}

private:
protected:
  std::string baseDir_;
  std::string suffix_;
  std::string tag_;
  std::string label_;
  bool isdata_;
  bool issignal_;
  bool isfastsim_;
  bool spimpose_;
  int color_;
  int lcolor_;
  int lwidth_;
  int lstyle_;
  int fill_;
  int marker_;
  int mcolor_;

  std::vector<SampleInfo> samples_;
  std::vector<std::string> missingFiles_;
};

class SampleReader
{
public:
  SampleReader(std::string fileName, std::string baseDir = "", std::string suffix = "");
  //~SampleReader();

  void printErrors(std::ostream&);
  std::vector<std::string> getAllFiles();
  TChain* getChain();
  THStack* getStack(std::string, std::string, std::string, int, double, double);
  TH1D* getHist(std::string, std::string, std::string, std::string, int, double, double);
  //void draw(std::string, std::string, std::string);
  doubleUnc getYield(std::string, std::string);

  SampleReader getData();
  SampleReader getMCBkg();
  SampleReader getMCSig();
  SampleReader getMC();

  // Iteration
  typedef typename std::vector<ProcessInfo>::iterator iterator;
  typedef typename std::vector<ProcessInfo>::const_iterator const_iterator;

  iterator begin() {return processes_.begin();}
  const_iterator begin() const {return processes_.begin();}
  const_iterator cbegin() const {return processes_.cbegin();}
  iterator end() {return processes_.end();}
  const_iterator end() const {return processes_.end();}
  const_iterator cend() const {return processes_.cend();}

private:
  SampleReader(): inputFile_(""), baseDir_(""), suffix_("") {};

protected:
  std::string inputFile_;
  std::string baseDir_;
  std::string suffix_;
  std::vector<ProcessInfo> processes_;
  std::vector<std::string> missingFiles_;
};

#endif
