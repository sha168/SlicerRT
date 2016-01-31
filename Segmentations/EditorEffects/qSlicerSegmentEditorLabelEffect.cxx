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

// Segmentations includes
#include "qSlicerSegmentEditorLabelEffect.h"
#include "qSlicerSegmentEditorLabelEffect_p.h"

#include "vtkOrientedImageData.h"
#include "vtkMRMLSegmentationNode.h"

// Qt includes
#include <QDebug>
#include <QCheckBox>
#include <QLabel>

// CTK includes
#include "ctkRangeWidget.h"

// VTK includes
#include <vtkMatrix4x4.h>

// MRML includes
#include "vtkMRMLVolumeNode.h"
#include "vtkMRMLTransformNode.h"

//-----------------------------------------------------------------------------
// qSlicerSegmentEditorLabelEffectPrivate methods

//-----------------------------------------------------------------------------
qSlicerSegmentEditorLabelEffectPrivate::qSlicerSegmentEditorLabelEffectPrivate(qSlicerSegmentEditorLabelEffect& object)
  : q_ptr(&object)
  , PaintOverCheckbox(NULL)
  , ThresholdPaintCheckbox(NULL)
  , ThresholdLabel(NULL)
  , ThresholdRangeWidget(NULL)
{
}

//-----------------------------------------------------------------------------
qSlicerSegmentEditorLabelEffectPrivate::~qSlicerSegmentEditorLabelEffectPrivate()
{
}

//-----------------------------------------------------------------------------
void qSlicerSegmentEditorLabelEffectPrivate::masterVolumeScalarRange(double& low, double& high)
{
  Q_Q(qSlicerSegmentEditorLabelEffect);

  low = 0.0;
  high = 0.0;

  vtkMRMLVolumeNode* masterVolumeNode = q->masterVolumeNode();
  if (!masterVolumeNode)
  {
    qCritical() << "qSlicerSegmentEditorLabelEffectPrivate::masterVolumeScalarRange: Failed to get master volume!";
    return;
  }
  if (masterVolumeNode->GetImageData())
  {
    double range[2] = {0.0, 0.0};
    masterVolumeNode->GetImageData()->GetScalarRange(range);
    low = range[0];
    high = range[1];
  }
}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// qSlicerSegmentEditorLabelEffect methods

//----------------------------------------------------------------------------
qSlicerSegmentEditorLabelEffect::qSlicerSegmentEditorLabelEffect(QObject* parent)
 : Superclass(parent)
 , d_ptr( new qSlicerSegmentEditorLabelEffectPrivate(*this) )
{
}

//----------------------------------------------------------------------------
qSlicerSegmentEditorLabelEffect::~qSlicerSegmentEditorLabelEffect()
{
}

//-----------------------------------------------------------------------------
void qSlicerSegmentEditorLabelEffect::setupOptionsFrame()
{
  Q_D(qSlicerSegmentEditorLabelEffect);

  d->PaintOverCheckbox = new QCheckBox("Paint over");
  d->PaintOverCheckbox->setToolTip("Allow effect to overwrite non-zero labels.");
  this->addOptionsWidget(d->PaintOverCheckbox);

  d->ThresholdPaintCheckbox = new QCheckBox("Threshold paint");
  d->ThresholdPaintCheckbox->setToolTip("Enable/Disable threshold mode for labeling");
  this->addOptionsWidget(d->ThresholdPaintCheckbox);

  d->ThresholdLabel = new QLabel("Threshold");
  d->ThresholdLabel->setToolTip("In threshold mode, the label will only be set if the background value is within this range.");
  this->addOptionsWidget(d->ThresholdLabel);

  d->ThresholdRangeWidget = new ctkRangeWidget();
  d->ThresholdRangeWidget->setSpinBoxAlignment(Qt::AlignTop); //TODO: 0xff?
  d->ThresholdRangeWidget->setSingleStep(0.01);
  double low = 0.0;
  double high = 0.0;
  d->masterVolumeScalarRange(low, high);
  d->ThresholdRangeWidget->setMinimum(low);
  d->ThresholdRangeWidget->setMaximum(high);
  this->addOptionsWidget(d->ThresholdRangeWidget);

  QObject::connect(d->PaintOverCheckbox, SIGNAL(clicked()), d, SLOT(updateMRMLFromGUI()));
  QObject::connect(d->ThresholdPaintCheckbox, SIGNAL(clicked()), d, SLOT(updateMRMLFromGUI()));
  QObject::connect(d->ThresholdRangeWidget, SIGNAL(valuesChanged(double,double)), d, SLOT(onThresholdValuesChange(double,double)));
}

//-----------------------------------------------------------------------------
void qSlicerSegmentEditorLabelEffect::setMRMLDefaults()
{
  this->setParameter(this->paintOverParameterName(), 1);
  this->setParameter(this->paintThresholdParameterName(), 0);
  this->setParameter(this->paintThresholdMinParameterName(), 0);
  this->setParameter(this->paintThresholdMaxParameterName(), 1000);
  this->setParameter(this->thresholdEnabledParameterName(), 1);
  this->setParameter(this->paintOverEnabledParameterName(), 1);
}

//-----------------------------------------------------------------------------
void qSlicerSegmentEditorLabelEffect::updateGUIFromMRML()
{
  Q_D(qSlicerSegmentEditorLabelEffect);

  d->PaintOverCheckbox->setChecked(this->integerParameter(this->paintOverParameterName()));
  d->ThresholdPaintCheckbox->setChecked(this->integerParameter(this->paintThresholdParameterName()));
  d->ThresholdRangeWidget->setMinimumValue(this->doubleParameter(this->paintThresholdMinParameterName()));
  d->ThresholdRangeWidget->setMaximumValue(this->doubleParameter(this->paintThresholdMaxParameterName()));

  d->PaintOverCheckbox->setVisible(this->integerParameter(this->paintOverEnabledParameterName()));
  d->ThresholdLabel->setVisible(this->integerParameter(this->thresholdEnabledParameterName()));
  d->ThresholdPaintCheckbox->setVisible(this->integerParameter(this->thresholdEnabledParameterName()));
  d->ThresholdRangeWidget->setVisible(this->integerParameter(this->thresholdEnabledParameterName()));
}

//-----------------------------------------------------------------------------
void qSlicerSegmentEditorLabelEffect::updateMRMLFromGUI()
{
  Q_D(qSlicerSegmentEditorLabelEffect);

  this->setParameter(this->paintOverParameterName(), (int)d->PaintOverCheckbox->isChecked());
  this->setParameter(this->paintThresholdParameterName(), (int)d->ThresholdPaintCheckbox->isChecked());
  this->setParameter(this->paintThresholdMinParameterName(), (double)d->ThresholdRangeWidget->minimumValue());
  this->setParameter(this->paintThresholdMaxParameterName(), (double)d->ThresholdRangeWidget->maximumValue());
}

//-----------------------------------------------------------------------------
void qSlicerSegmentEditorLabelEffect::ijkToRasMatrix(vtkMRMLVolumeNode* node, vtkMatrix4x4* ijkToRas)
{
  if (!node || !ijkToRas)
  {
    return;
  }

  node->GetIJKToRASMatrix(ijkToRas);

  vtkMRMLTransformNode* transformNode = node->GetParentTransformNode();
  if (transformNode)
  {
    if (transformNode->IsTransformToWorldLinear())
    {
      vtkSmartPointer<vtkMatrix4x4> volumeRasToWorldRas = vtkSmartPointer<vtkMatrix4x4>::New();
      transformNode->GetMatrixTransformToWorld(volumeRasToWorldRas);
      vtkMatrix4x4::Multiply4x4(volumeRasToWorldRas, ijkToRas, ijkToRas);
    }
    else
    {
      qCritical() << "qSlicerSegmentEditorLabelEffect::ijkToRasMatrix: Parent transform is non-linear, which cannot be handled! Skipping.";
    }
  }
}

//-----------------------------------------------------------------------------
void qSlicerSegmentEditorLabelEffect::ijkToRasMatrix(vtkOrientedImageData* image, vtkMRMLSegmentationNode* node, vtkMatrix4x4* ijkToRas)
{
  if (!image || !node || !ijkToRas)
  {
    return;
  }

  image->GetImageToWorldMatrix(ijkToRas);

  vtkMRMLTransformNode* transformNode = node->GetParentTransformNode();
  if (transformNode)
  {
    if (transformNode->IsTransformToWorldLinear())
    {
      vtkSmartPointer<vtkMatrix4x4> segmentationRasToWorldRas = vtkSmartPointer<vtkMatrix4x4>::New();
      transformNode->GetMatrixTransformToWorld(segmentationRasToWorldRas);
      vtkMatrix4x4::Multiply4x4(segmentationRasToWorldRas, ijkToRas, ijkToRas);
    }
    else
    {
      qCritical() << "qSlicerSegmentEditorLabelEffect::ijkToRasMatrix: Parent transform is non-linear, which cannot be handled! Skipping.";
    }
  }
}