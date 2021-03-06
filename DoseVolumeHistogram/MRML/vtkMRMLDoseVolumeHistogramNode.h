/*==============================================================================

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Csaba Pinter, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

#ifndef __vtkMRMLDoseVolumeHistogramNode_h
#define __vtkMRMLDoseVolumeHistogramNode_h

// MRML includes
#include <vtkMRML.h>
#include <vtkMRMLNode.h>

// STD includes
#include <vector>
#include <map>

#include "vtkSlicerDoseVolumeHistogramModuleMRMLExport.h"

class vtkMRMLScalarVolumeNode;
class vtkMRMLPlotChartNode;
class vtkMRMLSegmentationNode;
class vtkMRMLTableNode;

/// \ingroup SlicerRt_QtModules_DoseVolumeHistogram
/// 
/// Parameter set node for DVH computation.
/// Node references:
///   Parameter -> Dose Volume (DOSE_VOLUME_REFERENCE_ROLE)
///             -> Segmentation Node (SEGMENTATION_REFERENCE_ROLE)
///             -> Metrics Table (mandatory, unique to parameter) (DVH_METRICS_TABLE_REFERENCE_ROLE)
///             -> Chart (mandatory, unique to parameter) (CHART_REFERENCE_ROLE)
///   Metrics Table -> DVH Table nodes (DVH_ATTRIBUTE_PREFIX[DoseVolumeID]_[SegmentationNodeID]_SegmentID
///                    Generated by \sa AssembleDvhNodeReference
///   DVH Table -> Dose Volume (DOSE_VOLUME_REFERENCE_ROLE)
///             -> Segmentation Node (SEGMENTATION_REFERENCE_ROLE)
///             -> Metrics Table (DVH_METRICS_TABLE_REFERENCE_ROLE)
///                    
class VTK_SLICER_DOSEVOLUMEHISTOGRAM_MRML_EXPORT vtkMRMLDoseVolumeHistogramNode : public vtkMRMLNode
{
public:
  // DoseVolumeHistogram constants
  static const std::string DVH_ATTRIBUTE_PREFIX;
  static const char* DOSE_VOLUME_REFERENCE_ROLE;
  static const char* SEGMENTATION_REFERENCE_ROLE;
  static const char* DVH_METRICS_TABLE_REFERENCE_ROLE;

  // Column numbers of the default metrics in the table
  enum
  {
    MetricColumnVisible = 0,
    MetricColumnStructure,
    MetricColumnDoseVolume,
    MetricColumnVolumeCc,
    MetricColumnMeanDose,
    MetricColumnMinDose,
    MetricColumnMaxDose,
    NumberOfDefaultColumns
  };

public:
  static vtkMRMLDoseVolumeHistogramNode *New();
  vtkTypeMacro(vtkMRMLDoseVolumeHistogramNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /// Create instance of a GAD node. 
  virtual vtkMRMLNode* CreateNodeInstance() VTK_OVERRIDE;

  /// Set node attributes from name/value pairs 
  virtual void ReadXMLAttributes(const char** atts) VTK_OVERRIDE;

  /// Write this node's information to a MRML file in XML format. 
  virtual void WriteXML(ostream& of, int indent) VTK_OVERRIDE;

  /// Copy the node's attributes to this object 
  virtual void Copy(vtkMRMLNode *node) VTK_OVERRIDE;

  /// Get unique node XML tag name (like Volume, Model) 
  virtual const char* GetNodeTagName() VTK_OVERRIDE { return "DoseVolumeHistogram"; };

public:
  /// Get dose volume node
  vtkMRMLScalarVolumeNode* GetDoseVolumeNode();
  /// Set and observe dose volume node
  void SetAndObserveDoseVolumeNode(vtkMRMLScalarVolumeNode* node);

  /// Get segmentation node
  vtkMRMLSegmentationNode* GetSegmentationNode();
  /// Set and observe segmentation node
  void SetAndObserveSegmentationNode(vtkMRMLSegmentationNode* node);

  /// Get DVH metrics table node
  vtkMRMLTableNode* GetMetricsTableNode();

  /// Get chart node
  vtkMRMLPlotChartNode* GetChartNode();

  /// Get selected segment IDs
  void GetSelectedSegmentIDs(std::vector<std::string> &selectedSegmentIDs)
  {
    selectedSegmentIDs = this->SelectedSegmentIDs;
  }
  /// Set selected segment IDs
  void SetSelectedSegmentIDs(std::vector<std::string> selectedSegmentIDs)
  {
    this->SelectedSegmentIDs = selectedSegmentIDs;
  }

  /// Clear automatic oversampling factors map
  void ClearAutomaticOversamplingFactors()
  {
    this->AutomaticOversamplingFactors.clear();
  }
  /// Add automatic oversampling factor to map
  void AddAutomaticOversamplingFactor(std::string segmentID, double factor)
  {
    this->AutomaticOversamplingFactors[segmentID] = factor;
  }
  /// Get automatic oversampling factors
  void GetAutomaticOversamplingFactors(std::map<std::string, double> &factors)
  {
    factors = this->AutomaticOversamplingFactors;
  }

  /// Assemble DVH node reference role for current input selection and specific segment
  std::string AssembleDvhNodeReference(std::string segmentID);

  /// Collect DVH Table nodes belonging to this parameter set node
  void GetDvhTableNodes(std::vector<vtkMRMLTableNode*> &dvhTableNodes);

  /// Get/Set Show/Hide all checkbox state
  vtkGetMacro(ShowHideAll, int);
  vtkSetMacro(ShowHideAll, int);

  /// Get/Set input dose values for V metrics
  vtkGetStringMacro(VDoseValues);
  vtkSetStringMacro(VDoseValues);

  /// Get/Set show Cc for V metrics checkbox state
  vtkGetMacro(ShowVMetricsCc, bool);
  vtkSetMacro(ShowVMetricsCc, bool);
  vtkBooleanMacro(ShowVMetricsCc, bool);

  /// Get/Set show % for V metrics checkbox state
  vtkGetMacro(ShowVMetricsPercent, bool);
  vtkSetMacro(ShowVMetricsPercent, bool);
  vtkBooleanMacro(ShowVMetricsPercent, bool);

  /// Get input volume cc values for D metrics
  vtkGetStringMacro(DVolumeValuesCc);
  /// Set input volume cc values for D metrics
  vtkSetStringMacro(DVolumeValuesCc);

  /// Get input volume % values for D metrics
  vtkGetStringMacro(DVolumeValuesPercent);
  /// Set input volume % values for D metrics
  vtkSetStringMacro(DVolumeValuesPercent);

  /// Get show Gy for D metrics checkbox state
  vtkGetMacro(ShowDMetrics, bool);
  /// Set show Gy for D metrics checkbox state
  vtkSetMacro(ShowDMetrics, bool);
  /// Set show Gy for D metrics checkbox state
  vtkBooleanMacro(ShowDMetrics, bool);

  /// Get show dose volumes only checkbox state
  vtkGetMacro(ShowDoseVolumesOnly, bool);
  /// Set show dose volumes only checkbox state
  vtkSetMacro(ShowDoseVolumesOnly, bool);
  /// Set show dose volumes only checkbox state
  vtkBooleanMacro(ShowDoseVolumesOnly, bool);

  /// Get automatic oversampling flag
  vtkGetMacro(AutomaticOversampling, bool);
  /// Set automatic oversampling flag
  vtkSetMacro(AutomaticOversampling, bool);
  /// Set automatic oversampling flag
  vtkBooleanMacro(AutomaticOversampling, bool);

  /// Get fractional labelmap flag
  vtkGetMacro(UseFractionalLabelmap, bool);
  /// Set fractional labelmap flag
  vtkSetMacro(UseFractionalLabelmap, bool);
  /// Get fractional labelmap flag
  vtkBooleanMacro(UseFractionalLabelmap, bool);

  /// Get dose surface histogram flag
  vtkGetMacro(DoseSurfaceHistogram, bool);
  /// Set dose surface histogram flag
  vtkSetMacro(DoseSurfaceHistogram, bool);
  /// Get dose surface histogram flag
  vtkBooleanMacro(DoseSurfaceHistogram, bool);

  /// Get if the surface histogram should be calculated using internal/external voxels
  vtkGetMacro(UseInsideDoseSurface, bool);
  /// Set if the surface histogram should be calculated using internal/external voxels
  vtkSetMacro(UseInsideDoseSurface, bool);
  /// Get if the surface histogram should be calculated using internal/external voxels
  vtkBooleanMacro(UseInsideDoseSurface, bool);

protected:
  /// Set and observe DVH metrics table node
  /// Metrics table node is unique and mandatory for each DVH node, so it is created within the node.
  void SetAndObserveMetricsTableNode(vtkMRMLTableNode* node);

  /// Set and observe chart node
  /// Chart node is unique and mandatory for each DVH node, so it is created within the node.
  void SetAndObserveChartNode(vtkMRMLPlotChartNode* node);

protected:
  vtkMRMLDoseVolumeHistogramNode();
  ~vtkMRMLDoseVolumeHistogramNode();
  vtkMRMLDoseVolumeHistogramNode(const vtkMRMLDoseVolumeHistogramNode&);
  void operator=(const vtkMRMLDoseVolumeHistogramNode&);

protected:
  /// List of segment IDs selected in the chosen segmentation. If empty, then all segments are considered selected
  std::vector<std::string> SelectedSegmentIDs;

  /// State of Show/Hide all checkbox
  int ShowHideAll;

  /// Input dose values for V metrics
  char* VDoseValues;

  /// State of Show Cc for V metrics checkbox
  bool ShowVMetricsCc;

  /// State of Show % for V metrics checkbox
  bool ShowVMetricsPercent;

  /// Input volume cc values for D metrics
  char* DVolumeValuesCc;

  /// Input volume % values for D metrics
  char* DVolumeValuesPercent;

  /// State of Show Gy for D metrics checkbox
  bool ShowDMetrics;

  /// State of Show dose volumes only checkbox
  bool ShowDoseVolumesOnly;

  /// Flag determining whether automatic oversampling is used
  /// If on, then oversampling is automatically determined based on the segments and dose volume, and used
  /// for both dose and segmentation when computing DVH.
  bool AutomaticOversampling;

  /// Automatic oversampling factors stored for each selected segment.
  /// If oversampling is automatic then they need to be stored for reporting purposes.
  /// This property is not saved to the scene, as these are temporary values.
  std::map<std::string, double> AutomaticOversamplingFactors;

  /// Flag telling whether or not to use fractional labelmaps
  bool UseFractionalLabelmap;

  /// State of dose surface histogram checkbox
  bool DoseSurfaceHistogram;

  /// Whether to calculate the dose volume histogram from voxels inside/outside the structure
  bool UseInsideDoseSurface;
};

#endif
