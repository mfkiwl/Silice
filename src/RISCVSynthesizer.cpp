/*

    Silice FPGA language and compiler
    Copyright 2019, (C) Sylvain Lefebvre and contributors

    List contributors with: git shortlog -n -s -- <filename>

    GPLv3 license, see LICENSE_GPLv3 in Silice repo root

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <https://www.gnu.org/licenses/>.

(header_2_G)
*/
// -------------------------------------------------

#include "RISCVSynthesizer.h"
#include "Config.h"
#include "Utils.h"

#include <LibSL.h>
#include <filesystem>

// -------------------------------------------------

using namespace std;
using namespace Silice;
using namespace Silice::Utils;

// -------------------------------------------------

void RISCVSynthesizer::gatherTypeNfo(siliceParser::TypeContext *type, t_type_nfo &_nfo) const
{
  if (type->TYPE() != nullptr) {
    splitType(type->TYPE()->getText(), _nfo);
  } else {
    reportError(type->getSourceInterval(), (int)type->getStart()->getLine(), "[RISCV] complex types are not yet supported");
  }
}

// -------------------------------------------------

/// \brief gather module information from parsed grammar
void RISCVSynthesizer::gather(siliceParser::RiscvContext *riscv)
{
  siliceParser::InOutListContext *list = riscv->inOutList();
  for (auto io : list->inOrOut()) {
    if (io->input()) {
      t_inout_nfo nfo;
      nfo.name               = io->input()->IDENTIFIER()->getText();
      nfo.do_not_initialize  = true;
      gatherTypeNfo(io->input()->type(), nfo.type_nfo);
      m_Inputs.emplace_back(nfo);
      m_InputNames.insert(make_pair(nfo.name, (int)m_Inputs.size() - 1));
    } else if (io->output()) {
      if (io->output()->declarationVar()->IDENTIFIER() == nullptr) {
        reportError(io->getSourceInterval(), -1, "[RISCV] table outputs are not supported");
      } else {
        t_output_nfo nfo;
        nfo.name = io->output()->declarationVar()->IDENTIFIER()->getText();
        nfo.do_not_initialize = true;
        gatherTypeNfo(io->output()->declarationVar()->type(), nfo.type_nfo);
        m_Outputs.emplace_back(nfo);
        m_OutputNames.insert(make_pair(nfo.name, (int)m_Outputs.size() - 1));
      }
    } else if (io->inout()) {
      t_inout_nfo nfo;
      nfo.name = io->inout()->IDENTIFIER()->getText();
      nfo.do_not_initialize = true;
      splitType(io->inout()->TYPE()->getText(), nfo.type_nfo);
      m_InOuts.emplace_back(nfo);
      m_InOutNames.insert(make_pair(nfo.name, (int)m_InOuts.size() - 1));
    } else {
      reportError(io->getSourceInterval(), -1, "[RISCV] io type '%s' not supported");
    }
  }
}

// -------------------------------------------------

std::string RISCVSynthesizer::cblockToString(siliceParser::CblockContext *cblock) const
{
  return extractCodeBetweenTokens("", (int)cblock->getSourceInterval().a, (int)cblock->getSourceInterval().b);
}

// -------------------------------------------------

int RISCVSynthesizer::memorySize(siliceParser::RiscvContext *riscv) const
{
  if (riscv->riscvModifiers() != nullptr) {
    for (auto m : riscv->riscvModifiers()->riscvModifier()) {
      if (m->IDENTIFIER()->getText() == "mem") {
        if (m->NUMBER() == nullptr) {
          reportError(riscv->getSourceInterval(), -1, "[RISCV] memory size should be a number, got '%s'.", m->STRING()->getText().c_str());
        } else {
          int sz = atoi(m->NUMBER()->getText().c_str());
          if (sz <= 0 || (sz % 4) != 0) {
            reportError(riscv->getSourceInterval(), -1, "[RISCV] memory size (in bytes) should be > 0 and multiple of four (got %d).", sz);
          }
          return sz;
          break;
        }
      }
    }
  }
  // memory size is mandatory, issue an error if not found
  reportError(riscv->getSourceInterval(), -1, "[RISCV] no memory size specified, please use a modifier e.g. <mem=1024>");
  return 0;
}

// -------------------------------------------------

int RISCVSynthesizer::stackSize(siliceParser::RiscvContext *riscv) const
{
  if (riscv->riscvModifiers() != nullptr) {
    for (auto m : riscv->riscvModifiers()->riscvModifier()) {
      if (m->IDENTIFIER()->getText() == "stack") {
        if (m->NUMBER() == nullptr) {
          reportError(riscv->getSourceInterval(), -1, "[RISCV] stack size should be a number.");
        } else {
          int sz = atoi(m->NUMBER()->getText().c_str());
          if (sz <= 0 || (sz % 4) != 0) {
            reportError(riscv->getSourceInterval(), -1, "[RISCV] stack size (in bytes) should be > 0 and multiple of four (got %d).", sz);
          }
          return sz;
          break;
        }
      }
    }
  }
  // optional, return default size if not found
  return 128;
}

// -------------------------------------------------

std::string RISCVSynthesizer::coreName(siliceParser::RiscvContext *riscv) const
{
  if (riscv->riscvModifiers() != nullptr) {
    for (auto m : riscv->riscvModifiers()->riscvModifier()) {
      if (m->IDENTIFIER()->getText() == "core") {
        if (m->STRING() == nullptr) {
          reportError(riscv->getSourceInterval(), -1, "[RISCV] core name should be a string.");
        } else {
          string core = m->STRING()->getText();
          core = core.substr(1, core.length() - 2); // remove '"' and '"'
          return core;
        }
      }
    }
  }
  // optional, return default core if not found
  return "ice-v";
}

// -------------------------------------------------

std::string RISCVSynthesizer::archName(siliceParser::RiscvContext *riscv) const
{
  if (riscv->riscvModifiers() != nullptr) {
    for (auto m : riscv->riscvModifiers()->riscvModifier()) {
      if (m->IDENTIFIER()->getText() == "arch") {
        if (m->STRING() == nullptr) {
          reportError(riscv->getSourceInterval(), -1, "[RISCV] arch name should be a string.");
        } else {
          string arch = m->STRING()->getText();
          arch = arch.substr(1, arch.length() - 2); // remove '"' and '"'
          return arch;
        }
      }
    }
  }
  // optional, returns default if not found
  return "rv32i";
}

// -------------------------------------------------

std::string RISCVSynthesizer::defines(siliceParser::RiscvContext *riscv) const
{
  std::string defs;
  if (riscv->riscvModifiers() != nullptr) {
    for (auto m : riscv->riscvModifiers()->riscvModifier()) {
      if (m->NUMBER() != nullptr) {
        defs += "-D " + m->IDENTIFIER()->getText() + "=" + m->NUMBER()->getText() + " ";
      } else {
        string str = m->STRING()->getText();
        str = str.substr(1, str.length() - 2); // remove '"' and '"'
        defs += "-D " + m->IDENTIFIER()->getText() + "=\"\\\"" + str + "\\\"\" ";
      }
    }
  }
  return defs;
}

// -------------------------------------------------

static string normalizePath(const string& _path)
{
  string str = _path;
  for (auto &c : str) {
    if (c == '\\') {
      c = '/';
    }
  }
  return str;
}

// -------------------------------------------------

bool RISCVSynthesizer::isAccessTrigger(siliceParser::OutputContext *output, std::string& _accessed) const
{
  sl_assert(output->declarationVar()->IDENTIFIER() != nullptr);
  string oname = output->declarationVar()->IDENTIFIER()->getText();
  if (oname.substr(0, 3) != "on_") {
    return false;
  }
  string vname = oname.substr(3);
  // is there an input/output with this name?
  bool istrigger = isInput(vname) || isOutput(vname) || isInOut(vname);
  if (istrigger) {
    _accessed = vname;
  } else {
    warn(Standard, output->getSourceInterval(),-1,
      "variable '%s' starts with 'on_' which is indicates an access trigger,\n             but no corresponding input or output was found.",
      oname.c_str());
  }
  return istrigger;
}

// -------------------------------------------------

string RISCVSynthesizer::generateCHeader(siliceParser::RiscvContext *riscv) const
{

  // NOTE TODO: error checking for width and non supported IO types, additional error reporting

  ostringstream header;
  // bit indicating an IO (first after useful address width)
  int periph_bit = justHigherPow2(memorySize(riscv));
  // base address for input/outputs
  int ptr_base   = 1 << periph_bit;
  int ptr_next   = 4; // bit for the next IO
  for (auto io : riscv->inOutList()->inOrOut()) {
    auto input  = dynamic_cast<siliceParser::InputContext *>   (io->input());
    auto output = dynamic_cast<siliceParser::OutputContext *>  (io->output());
    auto inout  = dynamic_cast<siliceParser::InoutContext *>   (io->inout());
    string addr = string("=(int*)0x") + string(sprint("%x", ptr_base | ptr_next));
    if (input) {
      header << "volatile int *__ptr__" << input->IDENTIFIER()->getText() << addr << ';' << nxl;
      // getter
      header << "static inline int " << input->IDENTIFIER()->getText() << "() { "
             << "return *__ptr__" << input->IDENTIFIER()->getText() << "; }" << nxl;
    } else if (output) {
      sl_assert(output->declarationVar()->IDENTIFIER() != nullptr);
      header << "volatile int *__ptr__" << output->declarationVar()->IDENTIFIER()->getText() << addr << ';' << nxl;
      // setter
      header << "static inline void " << output->declarationVar()->IDENTIFIER()->getText() << "(int v) { "
        << "*__ptr__" << output->declarationVar()->IDENTIFIER()->getText() << "=v; }" << nxl;
    } else if (inout) {
      header << "volatile int *__ptr__" << inout->IDENTIFIER()->getText() << addr << ';' << nxl;
      // getter
      header << "static inline int " << inout->IDENTIFIER()->getText() << "() { "
        << "return *__ptr__" << inout->IDENTIFIER()->getText() << "; }" << nxl;
      // setter
      header << "static inline void " << inout->IDENTIFIER()->getText() << "(int v) { "
        << "*__ptr__" << inout->IDENTIFIER()->getText() << "=v; }" << nxl;
    } else {
      sl_assert(false); // RISC-V supports only input/output/inout   TODO error message
    }
    ptr_next = ptr_next << 1;
    if (ptr_next >= ptr_base) {
      reportError(riscv->getSourceInterval(), -1, "[RISCV] address bust not wide enough for the number of input/outputs (one bit per IO is required)");
    }
  }
  // concatenate core specific header
  header << Utils::fileToString((normalizePath(CONFIG.keyValues()["libraries_path"]) + "/riscv/" + coreName(riscv) + "/header.h").c_str());
  return header.str();
}

// -------------------------------------------------

string RISCVSynthesizer::generateSiliceCode(siliceParser::RiscvContext *riscv) const
{
  string io_decl;
  string io_select;
  string io_reads  = "32b0";
  string io_writes;
  string on_accessed;
  // build index
  std::map<string, int> io2idx;
  int idx = 0;
  for (auto io : riscv->inOutList()->inOrOut()) {
    auto input  = dynamic_cast<siliceParser::InputContext *>   (io->input());
    auto output = dynamic_cast<siliceParser::OutputContext *>  (io->output());
    auto inout  = dynamic_cast<siliceParser::InoutContext *>   (io->inout());
    string name;
    if (input) {
      name = input->IDENTIFIER()->getText();
    } else if (output) {
      sl_assert(output->declarationVar()->IDENTIFIER() != nullptr);
      name = output->declarationVar()->IDENTIFIER()->getText();
    } else if (inout) {
      name = inout->IDENTIFIER()->getText();
    } else {
      reportError(inout->getSourceInterval(), -1, "[RISC-V] only inputs / outputs are support");
    }
    io2idx.insert(make_pair(name, idx++));
  }
  // write code
  idx = 0;
  for (auto io : riscv->inOutList()->inOrOut()) {
    auto input  = dynamic_cast<siliceParser::InputContext *>   (io->input());
    auto output = dynamic_cast<siliceParser::OutputContext *>  (io->output());
    auto inout  = dynamic_cast<siliceParser::InoutContext *>   (io->inout());
    io_select   = io_select + "uint1 ior_" + std::to_string(idx) + " <:: prev_mem_addr[" + std::to_string(idx) + ",1]; ";
    io_select   = io_select + "uint1 iow_" + std::to_string(idx) + " <:  memio.addr[" + std::to_string(idx) + ",1]; ";
    string v, init;
    t_type_nfo type_nfo;
    if (input) {
      gatherTypeNfo(input->type(), type_nfo);
      v         = input->IDENTIFIER()->getText();
      io_reads  = io_reads  + " | (ior_" + std::to_string(idx) + " ? " + v + " : 32b0)";
      io_decl   = io_decl + "input ";
    } else if (output) {
      sl_assert(output->declarationVar()->IDENTIFIER() != nullptr);
      gatherTypeNfo(output->declarationVar()->type(), type_nfo);
      v = output->declarationVar()->IDENTIFIER()->getText();
      string accessed;
      if (!isAccessTrigger(output, accessed)) {
        // standard output
        io_writes   = io_writes + v + " = iow_" + std::to_string(idx) + " ? memio.wdata : " + v + "; ";
      } else {
        // access trigger
        sl_assert(io2idx.count(accessed) != 0);
        if (type_nfo.width != 1) {
          reportError(output->getSourceInterval(), -1,
            "[RISC-V] access trigger '%s' should be a uint1", v.c_str());
        }
        int accessed_idx = io2idx.at(accessed);
        if (isInput(accessed)) {
          on_accessed = on_accessed + v + " = io_read  & ior_" + std::to_string(accessed_idx) + "; ";
        } else if (isOutput(accessed)) {
          on_accessed = on_accessed + v + " = io_write & iow_" + std::to_string(accessed_idx) + "; ";
        } else {
          // TODO
          reportError(output->getSourceInterval(), -1, "[RISC-V] inout not yet supported");
        }
      }
      init = "(0)";
      io_decl = io_decl + "output ";
    } else if (inout) {
      splitType(inout->TYPE()->getText(), type_nfo);
      v         = inout->IDENTIFIER()->getText();
      // TODO
      reportError(inout->getSourceInterval(), -1, "[RISC-V] inout not yet supported");
    } else {
      // TODO error message, actual checks!
      reportError(inout->getSourceInterval(), -1, "[RISC-V] only inputs / outputs are support");
    }
    io_decl = io_decl + "uint" + std::to_string(type_nfo.width) + " " + v + init + ',';
    ++ idx;
  }
  ostringstream code;
  code << "$$addrW       = " << 1+justHigherPow2(memorySize(riscv)/4) << nxl;
  code << "$$memsz       = " << memorySize(riscv)/4 << nxl;
  code << "$$external    = " << justHigherPow2(memorySize(riscv))-2 << nxl;
  code << "$$arch        = '" << archName(riscv) << '\'' << nxl;
  code << "$$algorithm_name = '" << riscv->IDENTIFIER()->getText() << "'" << nxl;
  code << "$$dofile('" << normalizePath(CONFIG.keyValues()["libraries_path"]) + "/riscv/riscv-compile.lua');" << nxl;
  code << "$$meminit     = data_bram" << nxl;
  code << "$$io_decl     = [[" << io_decl << "]]" << nxl;
  code << "$$io_select   = [[" << io_select << "]]" << nxl;
  code << "$$io_reads    = [[" << io_reads << "]]" << nxl;
  code << "$$io_writes   = [[" << io_writes << "]]" << nxl;
  code << "$$on_accessed = [[" << on_accessed << "]]" << nxl;
  code << "$include(\"" << normalizePath(CONFIG.keyValues()["libraries_path"]) + "/riscv/" + coreName(riscv) + "/riscv-soc.ice\");" << nxl;
  return code.str();
}

// -------------------------------------------------

std::string RISCVSynthesizer::tempFileName()
{
  static int cnt = 0;
  static std::string key;
  if (key.empty()) {
    srand(time(NULL));
    for (int i = 0 ; i < 16 ; ++i) {
      key += (char)((int)'a'+(rand()%26));
    }
  }
  std::string tmp = std::filesystem::temp_directory_path().string()
                  + "/" + key + std::to_string(cnt++);
  return tmp;
}

// -------------------------------------------------

RISCVSynthesizer::RISCVSynthesizer(siliceParser::RiscvContext *riscv)
{
  m_Name = riscv->IDENTIFIER()->getText();
  gather(riscv);
  if (riscv->initList() != nullptr) {
    // table initializer with instructions
    reportError(riscv->initList()->getSourceInterval(), -1, "[RISC-V] init with instructions not yet supported");
  } else {
    // get core name
    std::string core = coreName(riscv);
    // compute stack start
    int stack_start = memorySize(riscv);
    // get stack size
    int stack_size = stackSize(riscv);
    // compile from inline source
    siliceParser::CblockContext *cblock = riscv->cblock();
    sl_assert(cblock != nullptr);
    string ccode = cblockToString(cblock);
    ccode = ccode.substr(1, ccode.size() - 2); // skip braces
    // write code to temp file
    string c_tempfile;
    {
      c_tempfile = tempFileName();
      c_tempfile = c_tempfile + ".c";
      ofstream codefile(c_tempfile);
      // append code header
      codefile << generateCHeader(riscv);
      // user provided code
      codefile << ccode << endl;
    }
    // generate Silice source
    string s_tempfile;
    {
      s_tempfile = tempFileName();
      s_tempfile = s_tempfile + ".ice";
      // produce source code
      ofstream silicefile(s_tempfile);
      silicefile << generateSiliceCode(riscv);
    }
    // generate linker script
    string l_tempfile;
    {
      l_tempfile = tempFileName();
      l_tempfile = l_tempfile + ".ld";
      // produce source code
      ofstream linkerscript(l_tempfile);
      int mem_size = memorySize(riscv) - stack_size;
      if (mem_size <= 0) {
        reportError(riscv->riscvModifiers()->getSourceInterval(), -1, "[RISC-V] memory size cannot fit stack (stack size is %d bytes, change with stack=SIZE)",stack_size);
      }
      linkerscript << "MEM_SIZE = " + std::to_string(mem_size) + ";\n"
        << Utils::fileToString((CONFIG.keyValues()["libraries_path"] + "/riscv/" + core + "/config_c.ld").c_str());
    }
    // get source file path
    std::string srcfile = Utils::getTokenSourceFileAndLine(Utils::getToken(riscv->RISCV()->getSourceInterval())).first;
    std::string path = std::filesystem::absolute(srcfile).remove_filename().string();
    // get defines from modifiers
    std::string defs = defines(riscv);
    // compile Silice source
    string exe = string(LibSL::System::Application::executablePath());
    string cmd =
      normalizePath(exe)
      + "/silice "
      + "-o " + m_Name + ".v "
      + "--export " + m_Name + " "
      + normalizePath(s_tempfile) + " "
      + "-D PATH=\"\\\"" + normalizePath(path) + "\\\"\" "
      + "-D SRC=\"\\\"" + normalizePath(c_tempfile) + "\\\"\" "
      + "-D CRT0=\"\\\"" + normalizePath(CONFIG.keyValues()["libraries_path"]) + "/riscv/" + core + "/crt0.s" + "\\\"\" "
      + "-D LD_CONFIG=\"\\\"" + normalizePath(l_tempfile) + "\\\"\" "
      + "-D STACK_START=" + std::to_string(stack_start) + " "
      + "-D STACK_SIZE=" + std::to_string(stack_size) + " "
      + defs + " "
      + "--framework " + normalizePath(CONFIG.keyValues()["frameworks_dir"]) + "/boards/bare/bare.v "
      ;
    system(cmd.c_str());
  }
}

// -------------------------------------------------
