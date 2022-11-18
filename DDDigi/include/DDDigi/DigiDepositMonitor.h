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
#ifndef DDDIGI_DIGIDEPOSITMONITOR_H
#define DDDIGI_DIGIDEPOSITMONITOR_H

/// Framework include files
#include <DDDigi/DigiData.h>
#include <DDDigi/DigiAction.h>
#include <DDDigi/DigiMonitorOptions.h>

/// Forward declarations
class TH1;
class TH2;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    template <typename HISTO> class Histogram  {
      friend class DigiKernel;
      friend class DigiDepositMonitor;
    protected:
      HISTO* hist  { nullptr };
    public:
      Histogram(HISTO* h) : hist(h) {}
    };

    /// Wrapper for 1 dimensional monitoring histograms
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class Histo1D : public Histogram<TH1>  {
    public:
      /// Add 1D histogram entry with weight
      void fill(double x, double weight=1.0);
    };

    /// Wrapper for 2 dimensional monitoring histograms
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class Histo2D : public Histogram<TH2>   {
    public:
      /// Add 2D histogram entry with weight
      void fill(double x, double y, double weight=1.0);
    };

    /// Monitor base class to histogram deposit property changes
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDepositMonitor : public DigiAction   {

    protected:
      using deposit_t = std::pair<CellID, EnergyDeposit>;

      std::function<void(std::size_t, std::size_t )> count_shift_monitor
	{ [](std::size_t, std::size_t) {} };
      std::function<void(const deposit_t& , double )> time_shift_monitor
	{ [](const deposit_t&, double) {} };
      std::function<void(const deposit_t& , double )> energy_shift_monitor
	{ [](const deposit_t&, double) {} };
      std::function<void(const deposit_t& , const Position&, const Position& )> position_shift_monitor
	{ [](const deposit_t&, const Position&, const Position&) {} };

      template <typename T> void declare_monitor_time(T* object, void (T::*pmf)(const deposit_t& , double))   {
	time_shift_monitor = std::bind(pmf, object, std::placeholders::_1, std::placeholders::_2);
      }
      template <typename T> void declare_monitor_energy(T* object, void (T::*pmf)(const deposit_t& , double))   {
	energy_shift_monitor = std::bind(pmf, object, std::placeholders::_1, std::placeholders::_2);
      }
      template <typename T> void declare_monitor_count(T* object, void (T::*pmf)(std::size_t, std::size_t))   {
	count_shift_monitor = std::bind(pmf, object, std::placeholders::_1, std::placeholders::_2);
      }
      template <typename T> void declare_monitor_position(T* object, void (T::*pmf)(const deposit_t& , const Position&, const Position&))   {
	position_shift_monitor = std::bind(pmf, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
      }

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiDepositMonitor);

      /// Default destructor
      virtual ~DigiDepositMonitor();

      /// Initializing function: overload for sub-classes to e.g. book histograms
      virtual void initialize();

      /// Book 1D histogram and register it to the kernel for output handling
      Histo1D book1D(const std::string& name, const std::string& title,
		     std::size_t nbin_x, double min_x, double max_x);

      /// Book 1D histogram and register it to the kernel for output handling
      Histo2D book2D(const std::string& name, const std::string& title,
		     std::size_t nbin_x, double min_x, double max_x,
		     std::size_t nbin_y, double min_y, double max_y);

    public:
      /// Standard constructor
      DigiDepositMonitor(const kernel_t& kernel, const std::string& name);

      inline void time_shift(const deposit_t& deposit, double change)    {
	time_shift_monitor(deposit, change);
      }
      inline void energy_shift(const deposit_t& deposit, double change)    {
	energy_shift_monitor(deposit, change);
      }
      inline void position_shift(const deposit_t& deposit, const Position& position, const Position& change)   {
	position_shift_monitor(deposit, position, change);
      }
      inline void count_shift(std::size_t start_value, std::size_t change)   {
	count_shift_monitor(start_value, change);
      }
    };

    /// Monitor base class to histogram the changes of the proper time of energy deposits
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDepositTimeMonitor : public DigiDepositMonitor   {

    protected:
      /// Property with the 1D histogram parameters to fill the histogram with deposits
      H1DParams histo1D_deposits { "", "", -1, 0e0, 0e0 };
      /// Property with the 1D histogram parameters to fill the histogram with deposit deltas
      H1DParams histo1D_delta    { "", "", -1, 0e0, 0e0 };

      /// Pointer to deposit histogram
      Histo1D  m_deposits   { nullptr };
      /// Pointer to delta histogram
      Histo1D  m_deltas     { nullptr };

      /// Initializing function: overload for sub-classes to e.g. book histograms
      virtual void initialize();

      /// Fill the monitoring histograms
      void monitor_time_shift(const deposit_t& deposit, double change);

    public:
      /// Standard constructor
      DigiDepositTimeMonitor(const kernel_t& kernel, const std::string& name);
    };

    /// Monitor base class to histogram energy deposit changes
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDepositEnergyMonitor : public DigiDepositMonitor   {
    protected:
      /// Property with the 1D histogram parameters to fill the histogram with deposits
      H1DParams histo1D_deposits { "", "", -1, 0e0, 0e0 };
      /// Property with the 1D histogram parameters to fill the histogram with deposit deltas
      H1DParams histo1D_delta    { "", "", -1, 0e0, 0e0 };

      /// Pointer to deposit histogram
      Histo1D m_deposits   { nullptr };
      /// Pointer to delta histogram
      Histo1D m_deltas     { nullptr };

      /// Initializing function: overload for sub-classes to e.g. book histograms
      virtual void initialize();

      /// Fill the monitoring histograms
      void monitor_energy_shift(const deposit_t& deposit, double change);

    public:
      /// Standard constructor
      DigiDepositEnergyMonitor(const kernel_t& kernel, const std::string& name);
    };

    /// Monitor base class to histogram the changes of the position of energy deposits
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDepositPositionMonitor : public DigiDepositMonitor   {

    protected:
      /// Property with the 1D histogram parameters to fill the histogram with deposits
      H1DParams histo1D_deposits { "", "", -1, 0e0, 0e0 };
      /// Property with the 1D histogram parameters to fill the histogram with deposit deltas
      H1DParams histo1D_delta    { "", "", -1, 0e0, 0e0 };

      /// Pointer to deposit histogram
      Histo1D m_deposits   { nullptr };
      /// Pointer to delta histogram
      Histo1D m_deltas     { nullptr };

      /// Initializing function: overload for sub-classes to e.g. book histograms
      virtual void initialize();

      /// Fill the monitoring histograms
      void monitor_position_shift(const deposit_t& deposit, const Position& position, const Position& change);

    public:
      /// Standard constructor
      DigiDepositPositionMonitor(const kernel_t& kernel, const std::string& name);
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIDEPOSITMONITOR_H
