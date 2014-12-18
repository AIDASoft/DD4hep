//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// $Id$
#ifndef DD4HEP_GEOMETRY_PANDORACONVERTER_H
#define DD4HEP_GEOMETRY_PANDORACONVERTER_H

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/GeoHandler.h"
#include "DD4hep/DetFactoryHelper.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   XML namespace declaration
   */
  namespace Geometry {

    /** @class PandoraConverter PandoraConverter.h XML/PandoraConverter.h
     *
     * Geometry converter from DD4hep to Geant 4.
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct PandoraConverter: public GeoHandler {
    protected:
      struct GeometryInfo: public GeoHandler::GeometryInfo {
        xml_doc_t doc;
        xml_elt_t doc_root, doc_calorimeters, doc_detector, doc_coil, doc_tracking;
        /// Helper constructor
        GeometryInfo();
      };

      /// Reference to detector description
      LCDD& m_lcdd;
      /// Data pointer
      GeometryInfo* m_dataPtr;

    public:

      /// Initializing Constructor
      PandoraConverter(LCDD& lcdd);

      /// Standard destructor
      virtual ~PandoraConverter();

      /// Create geometry conversion in Pandora XML format
      xml_doc_t create(DetElement top);

    };
  }    // End namespace XML
}      // End namespace DD4hep

#endif // DD4HEP_GEOMETRY_PANDORACONVERTER_H
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// $Id$

// Framework includes
#include "DD4hep/LCDD.h"
#include "DD4hep/GeoHandler.h"
#include "DD4hep/DetFactoryHelper.h"
#include "XML/DocumentHandler.h"

// C/C++ include files
#include <stdexcept>

using namespace DD4hep::Geometry;
using namespace DD4hep;
using namespace std;

/// Helper constructor
PandoraConverter::GeometryInfo::GeometryInfo()
  : doc(0), doc_root(0), doc_calorimeters(0), doc_detector(0), doc_coil(0), doc_tracking(0) {
}

/// Initializing Constructor
PandoraConverter::PandoraConverter(LCDD& lcdd)
  : m_lcdd(lcdd), m_dataPtr(0) {
}

/// Standard destructor
PandoraConverter::~PandoraConverter() {
  if (m_dataPtr)
    delete m_dataPtr;
  m_dataPtr = 0;
}

/// Create geometry conversion in Pandora XML format
xml_doc_t PandoraConverter::create(DetElement /* top */) {
  const char empty_xml[] = "<?xml version=\"1.0\" encoding=\"UTF-8\">\n"
    "<!--                                                               \n"
    "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
    "      ++++   Linear collider detector description in C++       ++++\n"
    "      ++++   DD4hep Detector description generator.            ++++\n"
    "      ++++                                                     ++++\n"
    "      ++++                              M.Frank CERN/LHCb      ++++\n"
    "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
    "-->\n"
    "<pandoraSetup>\n\0\0";
  XML::DocumentHandler docH;
  GeometryInfo& geo = *(m_dataPtr = new GeometryInfo);

  xml_elt_t elt(0);
  Header hdr = m_lcdd.header();
  geo.doc = docH.parse(empty_xml, sizeof(empty_xml));
  geo.doc_root = geo.doc.root();
  geo.doc_root.append(geo.doc_calorimeters = xml_elt_t(geo.doc, _Unicode(calorimeters)));
  geo.doc_root.append(geo.doc_detector = xml_elt_t(geo.doc, _Unicode(detector)));
  geo.doc_root.append(geo.doc_coil = xml_elt_t(geo.doc, _Unicode(coil)));
  geo.doc_root.append(geo.doc_tracking = xml_elt_t(geo.doc, _Unicode(tracking)));
  geo.doc_detector.setAttr(_Unicode(name), hdr.name());
  geo.doc_tracking.setAttr(_Unicode(innerR), "");
  geo.doc_tracking.setAttr(_Unicode(outerR), "");
  geo.doc_tracking.setAttr(_Unicode(z), "");

  return geo.doc;
}

static long create_lcdd(LCDD& /* lcdd */, int /* argc */, char** /* argv */) {
  throw runtime_error("The pandora xml conversion plugin is not yet implemented");
  return 0;
#if 0

  package org.lcsim.geometry.compact.converter.pandora;

  import static org.lcsim.geometry.Calorimeter.CalorimeterType.EM_BARREL;
  import static org.lcsim.geometry.Calorimeter.CalorimeterType.EM_ENDCAP;
  import static org.lcsim.geometry.Calorimeter.CalorimeterType.HAD_BARREL;
  import static org.lcsim.geometry.Calorimeter.CalorimeterType.HAD_ENDCAP;
  import static org.lcsim.geometry.Calorimeter.CalorimeterType.MUON_BARREL;
  import static org.lcsim.geometry.Calorimeter.CalorimeterType.MUON_ENDCAP;
  import hep.physics.particle.properties.ParticlePropertyManager;
  import hep.physics.particle.properties.ParticleType;
  import hep.physics.vec.BasicHep3Vector;
  import hep.physics.vec.Hep3Vector;

  import java.io.InputStream;
  import java.io.OutputStream;
  import java.text.DecimalFormat;
  import java.util.ArrayList;
  import java.util.List;
  import java.util.StringTokenizer;

  import javax.swing.filechooser.FileFilter;

  import org.jdom.Document;
  import org.jdom.Element;
  import org.jdom.output.Format;
  import org.jdom.output.XMLOutputter;
  import org.lcsim.conditions.ConditionsManager;
  import org.lcsim.conditions.ConditionsManager.ConditionsNotFoundException;
  import org.lcsim.conditions.ConditionsSet;
  import org.lcsim.detector.material.BetheBlochCalculator;
  import org.lcsim.detector.material.IMaterial;
  import org.lcsim.detector.material.MaterialStore;
  import org.lcsim.detector.solids.Tube;
  import org.lcsim.geometry.Calorimeter;
  import org.lcsim.geometry.Calorimeter.CalorimeterType;
  import org.lcsim.geometry.Detector;
  import org.lcsim.geometry.GeometryReader;
  import org.lcsim.geometry.compact.Subdetector;
  import org.lcsim.geometry.compact.converter.Converter;
  import org.lcsim.geometry.field.Solenoid;
  import org.lcsim.geometry.layer.Layer;
  import org.lcsim.geometry.layer.LayerSlice;
  import org.lcsim.geometry.layer.LayerStack;
  import org.lcsim.geometry.segmentation.AbstractCartesianGrid;
  import org.lcsim.geometry.subdetector.AbstractPolyhedraCalorimeter;
  import org.lcsim.geometry.subdetector.MultiLayerTracker;
  import org.lcsim.geometry.util.BaseIDDecoder;
  import org.lcsim.geometry.util.IDDescriptor;
  import org.lcsim.geometry.util.SamplingFractionManager;

  /**
   * This class converts from a compact detector description into slicPandora's
   * geometry input format.
   *
   * @author Jeremy McCormick <jeremym@slac.stanford.edu>
   * @version $Id$
   */
                                                        public class Main implements Converter
                                                          {
  private final static boolean DEBUG = false;

  // ConditionsManager instance.
                                            private ConditionsManager conditionsManager = ConditionsManager.defaultInstance();

                                            // Numerical formatting.
                                            static final DecimalFormat xlen = new DecimalFormat("#.########");
                                            static final DecimalFormat xfrac = new DecimalFormat("#.########");
                                            static final DecimalFormat xthick = new DecimalFormat("#.######");

                                            /**
                                             * A range of layers with associated EM and HAD sampling fractions.
                                             *
                                             * @author Jeremy McCormick <jeremym@slac.stanford.edu>
                                             */
                                            static class SamplingLayerRange
                                            {
                                              int lowerLayer;
                                              int upperLayer;
                                              double em;
                                              double had;

                                              SamplingLayerRange(int lowerLayer, int upperLayer, double em, double had)
                                              {
                                                this.lowerLayer = lowerLayer;
                                                this.upperLayer = upperLayer;
                                                this.em = em;
                                                this.had = had;
                                              }

    public boolean inRange(int layerNumber)
                                              {
                                                return layerNumber >= lowerLayer && layerNumber <= upperLayer;
                                              }

    public int getLowerLayer()
                                              {
                                                return lowerLayer;
                                              }

    public int getUpperLayer()
                                              {
                                                return upperLayer;
                                              }

    public double getEMSampling()
                                              {
                                                return em;
                                              }

    public double getHADSampling()
                                              {
                                                return had;
                                              }
                                            }

                                            /**
                                             * A list of SamplingLayerRange objects to represent the sampling for a
                                             * subdetector.
                                             *
                                             * @author Jeremy McCormick <jeremym@slac.stanford.edu>
                                             *
                                             */
                                            static class SamplingLayers extends ArrayList<SamplingLayerRange>
                                            {
    public SamplingLayers()
    {
    }

    public SamplingLayers(SamplingLayerRange range)
    {
      this.add(range);
    }

    public SamplingLayers(List<SamplingLayerRange> ranges)
      {
        this.addAll(ranges);
      }

    public SamplingLayerRange getSamplingLayerRange(int layern)
    {
      for (SamplingLayerRange range : this)
        {
          if (range.inRange(layern))
            return range;
        }
      return null;
    }
                                            }

                                            /**
                                             * Represents CalorimeterConditions for a single subdetector.
                                             *
                                             * @author Jeremy McCormick <jeremym@slac.stanford.edu>
                                             */
  private static class CalorimeterConditions
  {
    SamplingLayers samplingLayers;
    String name;
    double mipEnergy;
    double mipSigma;
    double mipCut;
    double timeCut;

                  public String toString()
    {
      StringBuffer buff = new StringBuffer();
      buff.append(name + '\n');
      for (SamplingLayerRange range : samplingLayers)
        {
          buff.append("[" + range.getLowerLayer() + " - " + range.getUpperLayer() + "]" + '\n');
          buff.append("    em = " + range.getEMSampling() + '\n');
          buff.append("    had = " + range.getHADSampling() + '\n');
        }

      return buff.toString();
    }

    public SamplingLayers getSamplingLayers()
    {
      return samplingLayers;
    }

    /**
     * Constructor that parses raw CalorimeterCalibration conditions for a
     * single subdetector.
     *
     * @param calorimeter
     * @param conditions
     */
    protected CalorimeterConditions(Calorimeter calorimeter, ConditionsSet conditions)
    {
      //System.out.println("conditions: " + calorimeter.getName());
      this.name = calorimeter.getName();

      // Figure out which layering conditions to use based on the
      // CalorimeterType.
      String layeringName = null;
      if (calorimeter.getCalorimeterType() == CalorimeterType.EM_BARREL || calorimeter.getCalorimeterType() == CalorimeterType.EM_ENDCAP)
        {
          layeringName = "ECalLayering";
        }
      else if (calorimeter.getCalorimeterType() == CalorimeterType.HAD_BARREL || calorimeter.getCalorimeterType() == CalorimeterType.HAD_ENDCAP)
        {
          layeringName = "HCalLayering";
        }
      else if (calorimeter.getCalorimeterType() == CalorimeterType.MUON_BARREL || calorimeter.getCalorimeterType() == CalorimeterType.MUON_ENDCAP)
        {
          layeringName = "MuonLayering";
        }
      else
        {
          throw new RuntimeException("Don't know how to handle CalorimeterConditions for " + calorimeter.getName() + ".");
        }

      String emName = null;
      String hadName = null;
      if (calorimeter.getCalorimeterType() == CalorimeterType.EM_BARREL || calorimeter.getCalorimeterType() == CalorimeterType.HAD_BARREL || calorimeter.getCalorimeterType() == CalorimeterType.MUON_BARREL)
        {
          emName = "EMBarrel_SF";
          hadName = "HadBarrel_SF";
        }
      else if (calorimeter.getCalorimeterType() == CalorimeterType.EM_ENDCAP || calorimeter.getCalorimeterType() == CalorimeterType.HAD_ENDCAP || calorimeter.getCalorimeterType() == CalorimeterType.MUON_ENDCAP)
        {
          emName = "EMEndcap_SF";
          hadName = "HadEndcap_SF";
        }

      if (emName == null || hadName == null)
        {
          throw new RuntimeException("Sampling fractions not found for " + calorimeter.getName() + ".");
        }

      String emSampling = conditions.getString(emName);
      String hadSampling = conditions.getString(hadName);
      List<Double> emSamplingFractions = new ArrayList<Double>();
      List<Double> hadSamplingFractions = new ArrayList<Double>();
      StringTokenizer tok = new StringTokenizer(emSampling, ",");
      while (tok.hasMoreTokens())
        {
          Double emSamplingFraction = Double.valueOf(tok.nextToken().trim());
          emSamplingFractions.add(emSamplingFraction);
        }
      tok = new StringTokenizer(hadSampling, ",");
      while (tok.hasMoreTokens())
        {
          Double hadSamplingFraction = Double.valueOf(tok.nextToken().trim());
          hadSamplingFractions.add(hadSamplingFraction);
        }

      String layering = conditions.getString(layeringName);
      tok = new StringTokenizer(layering, ",");
      List<Integer> layers = new ArrayList<Integer>();
      int maxLayer = calorimeter.getLayering().getLayerCount() - 1;
      while (tok.hasMoreTokens())
        {
          String nextToken = tok.nextToken().trim();
          int nextLayer = Integer.valueOf(nextToken);
          layers.add(nextLayer);
        }

      // FIXME Hack to get the correct starting index for the sampling
      // fractions. Ideally, the sampling fractions should be separated by subdetector name.
      int samplingIndex = 0;
      if (calorimeter.getCalorimeterType() == HAD_BARREL || calorimeter.getCalorimeterType() == HAD_ENDCAP)
        {
          samplingIndex = (new StringTokenizer(conditions.getString("ECalLayering"), ",").countTokens());
        }
      if (calorimeter.getCalorimeterType() == MUON_BARREL || calorimeter.getCalorimeterType() == MUON_ENDCAP)
        {
          samplingIndex = (new StringTokenizer(conditions.getString("ECalLayering"), ",").countTokens());
          samplingIndex += (new StringTokenizer(conditions.getString("HCalLayering"), ",").countTokens());
        }

      // System.out.println("    samplingIndex: " + samplingIndex);

      // Create the SamplingLayerRange list.
      samplingLayers = new SamplingLayers();
      for (int i = 0; i < layers.size(); i++)
        {
          // Figure out the layer range.
          int lowerLayer = layers.get(i);
          int upperLayer = 0;
          if (i + 1 > layers.size() - 1)
            upperLayer = maxLayer;
          else
            upperLayer = layers.get(i + 1) - 1;

          // Create the sampling layer range.
          double emSamplingFraction = emSamplingFractions.get(samplingIndex);
          double hadSamplingFraction = hadSamplingFractions.get(samplingIndex);
          SamplingLayerRange samplingLayerRange = new SamplingLayerRange(lowerLayer, upperLayer, emSamplingFraction, hadSamplingFraction);
          // System.out.println("    " + lowerLayer + " - " + upperLayer +
          // " : " + emSamplingFraction + ", " + hadSamplingFraction);

          samplingLayers.add(samplingLayerRange);

          ++samplingIndex;
        }

      // MIP energy.
      String mipCondition = null;
      String mipSigmaCondition = null;
      String mipCutCondition = null;

      // FIXME: Cleanup this ugliness.
      if (calorimeter.getCalorimeterType() == CalorimeterType.EM_BARREL || calorimeter.getCalorimeterType() == CalorimeterType.EM_ENDCAP)
        {
          mipCondition = "ECalMip_MPV";
          mipSigmaCondition = "ECalMip_sig";
          mipCutCondition = "ECalMip_Cut";
        }
      else if (calorimeter.getCalorimeterType() == CalorimeterType.HAD_BARREL || calorimeter.getCalorimeterType() == CalorimeterType.HAD_ENDCAP)
        {
          mipCondition = "HCalMip_MPV";
          mipSigmaCondition = "HCalMip_sig";
          mipCutCondition = "HCalMip_Cut";
        }
      else if (calorimeter.getCalorimeterType() == CalorimeterType.MUON_BARREL || calorimeter.getCalorimeterType() == CalorimeterType.MUON_ENDCAP)
        {
          mipCondition = "MuonMip_MPV";
          mipSigmaCondition = "MuonMip_sig";
          mipCutCondition = "MuonMip_Cut";
        }
      mipEnergy = conditions.getDouble(mipCondition);
      mipSigma = conditions.getDouble(mipSigmaCondition);
      mipCut = conditions.getDouble(mipCutCondition);
      timeCut = conditions.getDouble("timeCut");

      /*
       * System.out.println("    mipEnergy: " + mipEnergy);
       * System.out.println("    mipSigma: " + mipSigma);
       * System.out.println("    mipCut: " + mipCut);
       * System.out.println("    timeCut: " + timeCut);
       */
    }

    public SamplingLayerRange getSamplingLayerRange(int layer)
    {
      for (SamplingLayerRange layers : this.samplingLayers)
        {
          if (layers.inRange(layer))
            return layers;
        }
      return null;
    }

    public double getMipEnergy()
    {
      return mipEnergy;
    }

    public double getMipSigma()
    {
      return mipSigma;
    }

    public double getMipCut()
    {
      return mipCut;
    }

    public double getTimeCut()
    {
      return timeCut;
    }
  }

  public void convert(String inputFileName, InputStream in, OutputStream out) throws Exception
    {
      GeometryReader reader = new GeometryReader();
      Detector det = reader.read(in);
      String detectorName = det.getDetectorName();
      try
        {
          conditionsManager.setDetector(detectorName, 0);
        }
      catch (ConditionsNotFoundException x)
        {
          throw new RuntimeException("Failed to setup conditions system for detector: " + detectorName, x);
        }
      Document doc = convertDetectorToPandora(det);
      XMLOutputter outputter = new XMLOutputter();
      if (out != null)
        {
          outputter.setFormat(Format.getPrettyFormat());
          outputter.output(doc, out);
          out.close();
        }
    }

  public Document convertDetectorToPandora(Detector detector)
  {
    // Setup XML output document.
    Document outputDoc = new Document();
    Element root = new Element("pandoraSetup");
    outputDoc.setRootElement(root);
    Element calorimeters = new Element("calorimeters");
    root.addContent(calorimeters);

    // Add basic detector data element.
    Element detectorTag = new Element("detector");
    detectorTag.setAttribute("name", detector.getDetectorName());
    root.addContent(detectorTag);

    // Setup CalorimeterCalibration conditions.
    ConditionsSet calorimeterCalibration = null;
    try
      {
        calorimeterCalibration = conditionsManager.getConditions("CalorimeterCalibration");
      }
    catch (Exception x)
      {
      }
    boolean haveCalCalib = (calorimeterCalibration == null) ? false : true;

    // Process the subdetectors.
    for (Subdetector subdetector : detector.getSubdetectors().values())
      {
        //System.out.println(subdetector.getName());
        // Only handle calorimeters that are planar.
        if (subdetector instanceof AbstractPolyhedraCalorimeter)
          {
            Element calorimeter = new Element("calorimeter");
            AbstractPolyhedraCalorimeter polycal = (AbstractPolyhedraCalorimeter) subdetector;

            // Look for specific calorimeter types in the compact
            // description.
            Calorimeter.CalorimeterType calType = polycal.getCalorimeterType();
            if (calType.equals(HAD_BARREL) || calType.equals(HAD_ENDCAP) || calType.equals(EM_ENDCAP) || calType.equals(EM_BARREL) || calType.equals(MUON_BARREL) || calType.equals(MUON_ENDCAP))
              {
                // Set basic parameters in pandora calorimeter.
                calorimeter.setAttribute("type", Calorimeter.CalorimeterType.toString(calType));
                calorimeter.setAttribute("innerR", Double.toString(polycal.getInnerRadius()));
                calorimeter.setAttribute("innerZ", Double.toString(polycal.getInnerZ()));
                calorimeter.setAttribute("innerPhi", Double.toString(polycal.getSectionPhi()));
                calorimeter.setAttribute("innerSymmetryOrder", Double.toString(polycal.getNumberOfSides()));
                calorimeter.setAttribute("outerR", Double.toString(polycal.getOuterRadius()));
                calorimeter.setAttribute("outerZ", Double.toString(polycal.getOuterZ()));
                calorimeter.setAttribute("outerPhi", Double.toString(polycal.getSectionPhi()));
                calorimeter.setAttribute("outerSymmetryOrder", Double.toString(polycal.getNumberOfSides()));
                calorimeter.setAttribute("collection", subdetector.getReadout().getName());

                // Get the cell sizes from the segmentation.
                List<Double> cellSizes = getCellSizes(subdetector);

                // For endcaps, X is U, and Y is V.
                if (subdetector.isEndcap())
                  {
                    calorimeter.setAttribute("cellSizeU", Double.toString(cellSizes.get(0)));
                    calorimeter.setAttribute("cellSizeV", Double.toString(cellSizes.get(1)));
                  }
                // The UV mapping is flipped around for barrel.  X is V, and Y is U.
                else if (subdetector.isBarrel())
                  {
                    calorimeter.setAttribute("cellSizeU", Double.toString(cellSizes.get(1)));
                    calorimeter.setAttribute("cellSizeV", Double.toString(cellSizes.get(0)));
                  }

                // Create identifier description and add to subdet.
                calorimeter.addContent(makeIdentifierDescription(polycal));

                // Add the calorimeter.
                calorimeters.addContent(calorimeter);

                LayerStack layers = polycal.getLayering().getLayerStack();

                Element layersElem = new Element("layers");
                layersElem.setAttribute("nlayers", Integer.toString(layers.getNumberOfLayers()));

                calorimeter.addContent(layersElem);

                double layerD = 0.;

                if (polycal.isBarrel())
                  {
                    layerD = polycal.getInnerRadius();
                  }
                else if (polycal.isEndcap())
                  {
                    layerD = polycal.getInnerZ();
                  }

                CalorimeterConditions subdetectorCalorimeterConditions = null;

                if (haveCalCalib)
                  {
                    subdetectorCalorimeterConditions = new CalorimeterConditions((Calorimeter) subdetector, calorimeterCalibration);
                  }

                // Set MIP energy from calibration.
                if (haveCalCalib)
                  {
                    calorimeter.setAttribute("mipEnergy", xfrac.format(subdetectorCalorimeterConditions.getMipEnergy()));
                    calorimeter.setAttribute("mipSigma", xfrac.format(subdetectorCalorimeterConditions.getMipSigma()));
                    calorimeter.setAttribute("mipCut", xfrac.format(subdetectorCalorimeterConditions.getMipCut()));
                    calorimeter.setAttribute("timeCut", xfrac.format(subdetectorCalorimeterConditions.getTimeCut()));
                  }
                // Set MIP energy from Bethe-Bloche calculation.
                // TODO Check accuracy of this algorithm.
                else
                  {
                    List<LayerSlice> sensors = subdetector.getLayering().getLayerStack().getLayer(0).getSensors();
                    LayerSlice sensor = sensors.get(0);
                    IMaterial sensorMaterial = MaterialStore.getInstance().get(sensor.getMaterial().getName());

                    ParticleType particleType = ParticlePropertyManager.getParticlePropertyProvider().get(13);

                    Hep3Vector p = new BasicHep3Vector(-6.8641, -7.2721, 1.2168e-7);

                    double emip = BetheBlochCalculator.computeBetheBloch(sensorMaterial, p, particleType.getMass(), particleType.getCharge(), sensor.getThickness());

                    // Set MIP Energy from Bethe Bloche.
                    calorimeter.setAttribute("mipEnergy", xfrac.format(emip));

                    // Set defaults for CalCalib parameters.
                    calorimeter.setAttribute("mipSigma", "0");
                    calorimeter.setAttribute("mipCut", "0");
                    calorimeter.setAttribute("timeCut", xfrac.format(Double.MAX_VALUE));
                  }

                double totalX0 = 0;

                for (int i = 0; i < layers.getNumberOfLayers(); i++)
                  {
                    //System.out.println("  layer " + i);
                    Layer layer = layers.getLayer(i);

                    Element layerElem = new Element("layer");
                    layersElem.addContent(layerElem);

                    // Set radiation and interaction lengths.
                    double intLen = 0;
                    double radLen = 0;
                    for (int j = 0; j < layer.getNumberOfSlices(); j++)
                      {
                        LayerSlice slice = layer.getSlice(j);
                        //System.out.println("    slice " + j + " " + slice.getMaterial().getName());
                        double x0 = slice.getMaterial().getRadiationLength();
                        //System.out.println("      x0_mat_D="+x0);
                        //System.out.println("      x0_mat="+slice.getMaterial().getRadiationLength());
                        radLen += slice.getThickness() / (x0*10);
                        //System.out.println("      radLen="+radLen);

                        double lambda = slice.getMaterial().getNuclearInteractionLength();
                        intLen += slice.getThickness() / (lambda*10);
                      }
                    //System.out.println("    x0_lyr_tot=" + radLen);

                    totalX0 += radLen;

                    //System.out.println("    layer " + i + " " + radLen);

                    layerElem.setAttribute("radLen", xlen.format(radLen));
                    layerElem.setAttribute("intLen", xlen.format(intLen));

                    // Set distance to IP.
                    double layerD2 = layerD + layer.getThicknessToSensitiveMid();
                    layerElem.setAttribute("distanceToIp", xthick.format(layerD2));

                    // Set cell thickness.
                    layerElem.setAttribute("cellThickness", xthick.format(layer.getThickness()));

                    // Set EM and HAD sampling fractions from
                    // CalorimeterCalibration conditions, if present.
                    if (haveCalCalib)
                      {
                        SamplingLayerRange layerRange = subdetectorCalorimeterConditions.getSamplingLayerRange(i);
                        if (calType == EM_BARREL || calType == EM_ENDCAP)
                          {
                            layerElem.setAttribute("samplingFraction", xfrac.format(layerRange.getEMSampling()));
                          }
                        if (calType == HAD_BARREL || calType == HAD_ENDCAP)
                          {
                            layerElem.setAttribute("samplingFraction", xfrac.format(layerRange.getHADSampling()));
                          }
                        if (calType == MUON_BARREL || calType == MUON_ENDCAP)
                          {
                            layerElem.setAttribute("samplingFraction", xfrac.format(layerRange.getHADSampling()));
                          }
                        layerElem.setAttribute("emSamplingFraction", xfrac.format(layerRange.getEMSampling()));
                        layerElem.setAttribute("hadSamplingFraction", xfrac.format(layerRange.getHADSampling()));
                      }
                    // Set from base SamplingFraction conditions. May throw
                    // an exception if neither CalorimeterCalibration
                    // or SamplingFractions conditions exists.
                    else
                      {
                        double samplingFraction = SamplingFractionManager.defaultInstance().getSamplingFraction(subdetector, i);
                        layerElem.setAttribute("emSamplingFraction", xfrac.format(samplingFraction));
                        layerElem.setAttribute("hadSamplingFraction", xfrac.format(samplingFraction));
                      }

                    // Increment layer distance by thickness of layer.
                    layerD += layer.getThickness();
                  }

                //System.out.println("    X0 Sum = " + totalX0);
              }

            // Set digital flag.
            try
              {
                // Set digital attribute from conditions, if present.
                ConditionsSet conditions = conditionsManager.getConditions("SamplingFractions/" + subdetector.getName());
                boolean isDigital = conditions.getBoolean("digital");
                calorimeter.setAttribute("digital", String.valueOf(isDigital));
              }
            catch (Exception x)
              {
                calorimeter.setAttribute("digital", "false");
              }
          }
      }

    // TODO clean up the hard coded assumptions on coil geometry
    double coilRadLen = 0;
    double coilIntLen = 0;
    int coilLayers = 0;
    double coilInnerR = 0;
    double coilOuterR = 0;
    double bfield = 0;
    double coilMaxZ = 0;
    try
      {
        MultiLayerTracker c = (MultiLayerTracker) detector.getSubdetector("SolenoidCoilBarrel");
        if (c != null)
          {
            coilLayers = c.getNumberOfLayers();
            coilInnerR = c.getInnerR()[0];
            coilOuterR = c.getInnerR()[coilLayers-1] + c.getLayerThickness(coilLayers-1);
            for (int layern = 0; layern != c.getNumberOfLayers(); layern++)
              {
                for (LayerSlice slice : c.getLayer(layern).getSlices())
                  {
                    double x0 = slice.getMaterial().getRadiationLength();
                    double sliceRadLen = slice.getThickness() / (x0*10);
                    double lambda = slice.getMaterial().getNuclearInteractionLength();
                    double sliceIntLen = slice.getThickness() / (lambda*10);

                    coilRadLen += sliceRadLen;
                    coilIntLen += sliceIntLen;
                  }
              }
            //calculate average interaction/radiation length in coil material
            coilRadLen = coilRadLen/(coilOuterR-coilInnerR);
            coilIntLen = coilIntLen/(coilOuterR-coilInnerR);
          }
      }
    catch (ClassCastException e)
      {
        throw new RuntimeException(e);
      }
    try
      {
        Solenoid s = (Solenoid) detector.getFields().get("GlobalSolenoid");
        if (s != null)
          {
            bfield = s.getField(new BasicHep3Vector(0, 0, 0)).z();
            coilMaxZ = s.getZMax();
          }
      }
    catch (ClassCastException e)
      {
        throw new RuntimeException(e);
      }

    Element coil = new Element("coil");
    coil.setAttribute("radLen", xlen.format(coilRadLen));
    coil.setAttribute("intLen", xlen.format(coilIntLen));
    coil.setAttribute("innerR", Double.toString(coilInnerR));
    coil.setAttribute("outerR", Double.toString(coilOuterR));
    coil.setAttribute("z", Double.toString(coilMaxZ));
    coil.setAttribute("bfield", Double.toString(bfield));
    root.addContent(coil);

    Tube tube = (Tube) detector.getTrackingVolume().getLogicalVolume().getSolid();
    Element tracking = new Element("tracking");
    tracking.setAttribute("innerR", Double.toString(tube.getInnerRadius()));
    tracking.setAttribute("outerR", Double.toString(tube.getOuterRadius()));
    tracking.setAttribute("z", Double.toString(tube.getZHalfLength()));
    root.addContent(tracking);

    return outputDoc;
  }

  Element makeIdentifierDescription(Subdetector subdet)
  {
    IDDescriptor descr = subdet.getIDDecoder().getIDDescription();
    Element id = new Element("id");
    for (int i = 0, j = descr.fieldCount(); i < j; i++)
      {
        Element field = new Element("field");
        field.setAttribute("name", descr.fieldName(i));
        field.setAttribute("length", Integer.toString(descr.fieldLength(i)));
        field.setAttribute("start", Integer.toString(descr.fieldStart(i)));
        field.setAttribute("signed", Boolean.toString(descr.isSigned(i)));

        id.addContent(field);
      }
    return id;
  }

  private List<Double> getCellSizes(Subdetector subdetector)
    {
      List<Double> cellSizes = new ArrayList<Double>();
      BaseIDDecoder dec = (BaseIDDecoder) subdetector.getReadout().getIDDecoder();
      if (dec instanceof AbstractCartesianGrid)
        {
          AbstractCartesianGrid cgrid = (AbstractCartesianGrid) dec;
          if (cgrid.getGridSizeX() != 0)
            {
              cellSizes.add(cgrid.getGridSizeX());
            }
          if (cgrid.getGridSizeY() != 0)
            {
              cellSizes.add(cgrid.getGridSizeY());
            }
          if (cgrid.getGridSizeZ() != 0)
            {
              cellSizes.add(cgrid.getGridSizeZ());
            }
        }
      if (cellSizes.size() != 2)
        throw new RuntimeException("Only 2 cell dimensions are allowed.");
      return cellSizes;
    }

  public String getOutputFormat()
  {
    return "pandora";
  }

  public FileFilter getFileFilter()
  {
    return new PandoraFileFilter();
  }

  private static class PandoraFileFilter extends FileFilter
    {

    public boolean accept(java.io.File file)
      {
        return file.getName().endsWith(".xml");
      }

    public String getDescription()
    {
      return "Pandora Geometry file (*.xml)";
    }
    }
                                                          }
#endif
}
DECLARE_APPLY(DD4hepGeometry2PANDORA,create_lcdd)
