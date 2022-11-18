//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

/// Framework include files
#include <DD4hep/InstanceCount.h>
#include <DDDigi/DigiKernel.h>
#include <DDDigi/DigiDepositMonitor.h>

/// ROOT include filess
#include <TH1F.h>
#include <TH2F.h>

/// C/C++ include files

using namespace dd4hep::digi;

/// Add 1D histogram entry with weight
void Histo1D::fill(double x, double weight)    {
  hist->Fill(x, weight);
}

/// Add 2D histogram entry with weight
void Histo2D::fill(double x, double y, double weight)    {
  hist->Fill(x, y, weight);
}

/// Standard constructor
DigiDepositMonitor::DigiDepositMonitor(const DigiKernel& krnl, const std::string& nam)
  : DigiAction(krnl, nam)
{
  m_kernel.register_initialize(std::bind(&DigiDepositMonitor::initialize,this));
  InstanceCount::increment(this);
}

/// Default destructor
DigiDepositMonitor::~DigiDepositMonitor() {
  InstanceCount::decrement(this);
}

/// Initializing function: compute values which depend on properties
void DigiDepositMonitor::initialize()    {
}

/// Book 1D histogram and register it to the kernel for output handling
Histo1D DigiDepositMonitor::book1D(const std::string& name, const std::string& title,
				   std::size_t nbin_x, double min_x, double max_x)   {
  auto* h = new TH1F(name.c_str(), title.c_str(), nbin_x, min_x, max_x);
  m_kernel.register_monitor(this, h);
  return { h };
}

/// Book 1D histogram and register it to the kernel for output handling
Histo2D DigiDepositMonitor::book2D(const std::string& name, const std::string& title,
				   std::size_t nbin_x, double min_x, double max_x,
				   std::size_t nbin_y, double min_y, double max_y)   {
  auto* h = new TH2F(name.c_str(), title.c_str(), nbin_x, min_x, max_x, nbin_y, min_y, max_y);
  m_kernel.register_monitor(this, h);
  return { h };
}

/// Standard constructor
DigiDepositEnergyMonitor::DigiDepositEnergyMonitor(const kernel_t& kernel, const std::string& name)
  : DigiDepositMonitor(kernel, name)
{
  declareProperty("histo1D_deposits", histo1D_deposits);
  declareProperty("histo1D_delta",    histo1D_delta);
}

/// Initializing function: overload for sub-classes to e.g. book histograms
void DigiDepositEnergyMonitor::initialize()    {
  declare_monitor_energy(this, &DigiDepositEnergyMonitor::monitor_energy_shift);
  m_deltas = book1D(histo1D_delta.name.c_str(), histo1D_delta.title.c_str(), 
		    histo1D_delta.nbin_x, histo1D_delta.min_x, histo1D_delta.max_x);
  m_deposits = book1D(histo1D_deposits.name.c_str(), histo1D_deposits.title.c_str(), 
		      histo1D_deposits.nbin_x, histo1D_deposits.min_x, histo1D_deposits.max_x);
}

/// Fill the monitoring histograms
void DigiDepositEnergyMonitor::monitor_energy_shift(const deposit_t& deposit, double change)   {
  m_deposits.fill(deposit.second.deposit, 1e0);
  m_deltas.fill(change, 1e0);
}

/// Standard constructor
DigiDepositTimeMonitor::DigiDepositTimeMonitor(const kernel_t& kernel, const std::string& name)
  : DigiDepositMonitor(kernel, name)
{
  declareProperty("histo1D_deposits", histo1D_deposits);
  declareProperty("histo1D_delta",    histo1D_delta);
}

/// Initializing function: overload for sub-classes to e.g. book histograms
void DigiDepositTimeMonitor::initialize()    {
  declare_monitor_time(this, &DigiDepositTimeMonitor::monitor_time_shift);
  m_deltas = book1D(histo1D_delta.name.c_str(), histo1D_delta.title.c_str(), 
		    histo1D_delta.nbin_x, histo1D_delta.min_x, histo1D_delta.max_x);
  m_deposits = book1D(histo1D_deposits.name.c_str(), histo1D_deposits.title.c_str(), 
		      histo1D_deposits.nbin_x, histo1D_deposits.min_x, histo1D_deposits.max_x);
}

/// Fill the monitoring histograms
void DigiDepositTimeMonitor::monitor_time_shift(const deposit_t& deposit, double change)   {
  m_deposits.fill(deposit.second.time, 1e0);
  m_deltas.fill(change, 1e0);
}

/// Standard constructor
DigiDepositPositionMonitor::DigiDepositPositionMonitor(const kernel_t& kernel, const std::string& name)
  : DigiDepositMonitor(kernel, name)
{
  declareProperty("histo1D_deposits", histo1D_deposits);
  declareProperty("histo1D_delta",    histo1D_delta);
}

/// Initializing function: overload for sub-classes to e.g. book histograms
void DigiDepositPositionMonitor::initialize()    {
  declare_monitor_position(this, &DigiDepositPositionMonitor::monitor_position_shift);
  m_deltas = book1D(histo1D_delta.name.c_str(), histo1D_delta.title.c_str(), 
		    histo1D_delta.nbin_x, histo1D_delta.min_x, histo1D_delta.max_x);
  m_deposits = book1D(histo1D_deposits.name.c_str(), histo1D_deposits.title.c_str(), 
		      histo1D_deposits.nbin_x, histo1D_deposits.min_x, histo1D_deposits.max_x);
}

/// Fill the monitoring histograms
void DigiDepositPositionMonitor::monitor_position_shift(const deposit_t&, const Position& position, const Position& change)   {
  m_deposits.fill(std::sqrt(position.Mag2()), 1e0);
  m_deltas.fill(std::sqrt(change.Mag2()), 1e0);
}
