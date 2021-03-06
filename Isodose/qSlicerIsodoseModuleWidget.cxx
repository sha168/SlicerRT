/*==============================================================================

  Copyright (c) Radiation Medicine Program, University Health Network,
  Princess Margaret Hospital, Toronto, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Kevin Wang, Princess Margaret Cancer Centre 
  and was supported by Cancer Care Ontario (CCO)'s ACRU program 
  with funds provided by the Ontario Ministry of Health and Long-Term Care
  and Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO).

==============================================================================*/

// Qt includes
#include <QCheckBox>
#include <QDebug>

// SlicerQt includes
#include "qSlicerIsodoseModuleWidget.h"
#include "ui_qSlicerIsodoseModule.h"
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"

// SlicerRtCommon includes
#include "vtkSlicerRtCommon.h"

// qMRMLWidget includes
#include "qMRMLThreeDView.h"
#include "qMRMLThreeDWidget.h"
#include "qMRMLSliceWidget.h"
#include "qMRMLSliceView.h"

// Isodose includes
#include "vtkSlicerIsodoseModuleLogic.h"
#include "vtkMRMLIsodoseNode.h"
#include "vtkSlicerRTScalarBarActor.h"

// MRML includes
#include <vtkMRMLColorTableNode.h>
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelHierarchyNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSubjectHierarchyNode.h>

// VTK includes
#include <vtkColorTransferFunction.h>
#include <vtkLookupTable.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkScalarBarWidget.h>
#include <vtkVersion.h>

//-----------------------------------------------------------------------------
/// \ingroup SlicerRt_QtModules_Isodose
class qSlicerIsodoseModuleWidgetPrivate: public Ui_qSlicerIsodoseModule
{
  Q_DECLARE_PUBLIC(qSlicerIsodoseModuleWidget);
protected:
  qSlicerIsodoseModuleWidget* const q_ptr;
public:
  qSlicerIsodoseModuleWidgetPrivate(qSlicerIsodoseModuleWidget &object);
  ~qSlicerIsodoseModuleWidgetPrivate();

  vtkSlicerIsodoseModuleLogic* logic() const;

  vtkScalarBarWidget* ScalarBarWidget;
  vtkScalarBarWidget* ScalarBarWidget2DRed;
  vtkScalarBarWidget* ScalarBarWidget2DYellow;
  vtkScalarBarWidget* ScalarBarWidget2DGreen;

  vtkSlicerRTScalarBarActor* ScalarBarActor;
  vtkSlicerRTScalarBarActor* ScalarBarActor2DRed;
  vtkSlicerRTScalarBarActor* ScalarBarActor2DYellow;
  vtkSlicerRTScalarBarActor* ScalarBarActor2DGreen;

  std::vector<vtkScalarBarWidget*> ScalarBarWidgets;
};

//-----------------------------------------------------------------------------
// qSlicerIsodoseModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerIsodoseModuleWidgetPrivate::qSlicerIsodoseModuleWidgetPrivate(qSlicerIsodoseModuleWidget& object)
  : q_ptr(&object)
{
  // 3D view scalar bar
  this->ScalarBarActor = vtkSlicerRTScalarBarActor::New();
  this->ScalarBarWidget = vtkScalarBarWidget::New();
  this->ScalarBarWidget->SetScalarBarActor(this->ScalarBarActor);
  this->ScalarBarWidgets.push_back(this->ScalarBarWidget);
  
  // 2D views scalar bar
  this->ScalarBarActor2DRed = vtkSlicerRTScalarBarActor::New();
  this->ScalarBarWidget2DRed = vtkScalarBarWidget::New();
  this->ScalarBarWidget2DRed->SetScalarBarActor(this->ScalarBarActor2DRed);
  this->ScalarBarWidgets.push_back(this->ScalarBarWidget2DRed);

  this->ScalarBarActor2DYellow = vtkSlicerRTScalarBarActor::New();
  this->ScalarBarWidget2DYellow = vtkScalarBarWidget::New();
  this->ScalarBarWidget2DYellow->SetScalarBarActor(this->ScalarBarActor2DYellow);
  this->ScalarBarWidgets.push_back(this->ScalarBarWidget2DYellow);

  this->ScalarBarActor2DGreen = vtkSlicerRTScalarBarActor::New();
  this->ScalarBarWidget2DGreen = vtkScalarBarWidget::New();
  this->ScalarBarWidget2DGreen->SetScalarBarActor(this->ScalarBarActor2DGreen);
  this->ScalarBarWidgets.push_back(this->ScalarBarWidget2DGreen);

  for (std::vector<vtkScalarBarWidget*>::iterator it = this->ScalarBarWidgets.begin();
    it != this->ScalarBarWidgets.end(); ++it)
  {
    vtkSlicerRTScalarBarActor* actor = vtkSlicerRTScalarBarActor::SafeDownCast(
      (*it)->GetScalarBarActor() );
    actor->SetOrientationToVertical();
    actor->SetNumberOfLabels(0);
    actor->SetMaximumNumberOfColors(0);
    actor->SetTitle("Dose(Gy)");
    actor->SetLabelFormat(" %s");
    actor->UseAnnotationAsLabelOn();
    // It's a 2d actor, position it in screen space by percentages
    actor->SetPosition(0.1, 0.1);
    actor->SetWidth(0.1);
    actor->SetHeight(0.8);
  }
}

//-----------------------------------------------------------------------------
qSlicerIsodoseModuleWidgetPrivate::~qSlicerIsodoseModuleWidgetPrivate()
{
  if (this->ScalarBarWidget)
  {
    this->ScalarBarWidget->Delete();
    this->ScalarBarWidget = 0;
  }
  if (this->ScalarBarActor)
  {
    this->ScalarBarActor->Delete();
    this->ScalarBarActor = 0;
  }
  if (this->ScalarBarWidget2DRed)
  {
    this->ScalarBarWidget2DRed->Delete();
    this->ScalarBarWidget2DRed = 0;
  }
  if (this->ScalarBarActor2DRed)
  {
    this->ScalarBarActor2DRed->Delete();
    this->ScalarBarActor2DRed = 0;
  }
  if (this->ScalarBarWidget2DYellow)
  {
    this->ScalarBarWidget2DYellow->Delete();
    this->ScalarBarWidget2DYellow = 0;
  }
  if (this->ScalarBarActor2DYellow)
  {
    this->ScalarBarActor2DYellow->Delete();
    this->ScalarBarActor2DYellow = 0;
  }
  if (this->ScalarBarWidget2DGreen)
  {
    this->ScalarBarWidget2DGreen->Delete();
    this->ScalarBarWidget2DGreen = 0;
  }
  if (this->ScalarBarActor2DGreen)
  {
    this->ScalarBarActor2DGreen->Delete();
    this->ScalarBarActor2DGreen = 0;
  }
}

//-----------------------------------------------------------------------------
vtkSlicerIsodoseModuleLogic* qSlicerIsodoseModuleWidgetPrivate::logic() const
{
  Q_Q(const qSlicerIsodoseModuleWidget);
  return vtkSlicerIsodoseModuleLogic::SafeDownCast(q->logic());
} 

//-----------------------------------------------------------------------------
// qSlicerIsodoseModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerIsodoseModuleWidget::qSlicerIsodoseModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerIsodoseModuleWidgetPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
qSlicerIsodoseModuleWidget::~qSlicerIsodoseModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerIsodoseModuleWidget);

  this->Superclass::setMRMLScene(scene);

  qvtkReconnect( d->logic(), scene, vtkMRMLScene::EndImportEvent, this, SLOT(onSceneImportedEvent()) );

  // Find parameters node or create it if there is no one in the scene
  if (scene && d->MRMLNodeComboBox_ParameterSet->currentNode() == 0)
  {
    vtkMRMLNode* node = scene->GetNthNodeByClass(0, "vtkMRMLIsodoseNode");
    if (node)
    {
      this->setParameterNode(node);
    }
    else 
    {
      vtkSmartPointer<vtkMRMLIsodoseNode> newNode = vtkSmartPointer<vtkMRMLIsodoseNode>::New();
      this->mrmlScene()->AddNode(newNode);
      this->setParameterNode(newNode);
    }
  }
}

//-----------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::onSceneImportedEvent()
{
  this->onEnter();
}

//-----------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::enter()
{
  this->onEnter();
  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::onEnter()
{
  if (!this->mrmlScene())
  {
    qCritical() << Q_FUNC_INFO << ": Invalid scene";
    return;
  }

  Q_D(qSlicerIsodoseModuleWidget);

  // First check the logic if it has a parameter node
  if (!d->logic())
  {
    qCritical() << Q_FUNC_INFO << ": Invalid logic";
    return;
  }
  vtkMRMLIsodoseNode* paramNode = vtkMRMLIsodoseNode::SafeDownCast(d->MRMLNodeComboBox_ParameterSet->currentNode());

  // If we have a parameter node select it
  if (!paramNode)
  {
    vtkMRMLNode* node = this->mrmlScene()->GetNthNodeByClass(0, "vtkMRMLIsodoseNode");
    if (node)
    {
      this->setParameterNode(node);
    }
    else 
    {
      vtkSmartPointer<vtkMRMLIsodoseNode> newNode = vtkSmartPointer<vtkMRMLIsodoseNode>::New();
      this->mrmlScene()->AddNode(newNode);
      this->setParameterNode(newNode);
    }
  }
  else
  {
    this->updateWidgetFromMRML();
  }
}

//-----------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerIsodoseModuleWidget);

  vtkMRMLIsodoseNode* paramNode = vtkMRMLIsodoseNode::SafeDownCast(d->MRMLNodeComboBox_ParameterSet->currentNode());
  if (paramNode && this->mrmlScene())
  {
    d->MRMLNodeComboBox_ParameterSet->setCurrentNode(paramNode);

    if (paramNode->GetDoseVolumeNode())
    {
      d->MRMLNodeComboBox_DoseVolume->setCurrentNode(paramNode->GetDoseVolumeNode());

      vtkMRMLColorTableNode* colorTableNode = paramNode->GetColorTableNode();       
      if (!colorTableNode)
      {
        qCritical() << Q_FUNC_INFO << ": Invalid color table node";
        return;
      }
      d->spinBox_NumberOfLevels->setValue(colorTableNode->GetNumberOfColors());
    }
    else
    {
      this->doseVolumeNodeChanged(d->MRMLNodeComboBox_DoseVolume->currentNode());
    }

    this->updateScalarBarsFromSelectedColorTable();

    d->checkBox_Isoline->setChecked(paramNode->GetShowIsodoseLines());
    d->checkBox_Isosurface->setChecked(paramNode->GetShowIsodoseSurfaces());
  }
}

//-----------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::onLogicModified()
{
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::setup()
{
  Q_D(qSlicerIsodoseModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // Show only dose volumes in the dose volume combobox by default
  d->MRMLNodeComboBox_DoseVolume->addAttribute( QString("vtkMRMLScalarVolumeNode"), vtkSlicerRtCommon::DICOMRTIMPORT_DOSE_VOLUME_IDENTIFIER_ATTRIBUTE_NAME.c_str());

  // Make connections
  connect( d->MRMLNodeComboBox_ParameterSet, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT( setParameterNode(vtkMRMLNode*) ) );
  connect( d->MRMLNodeComboBox_DoseVolume, SIGNAL( currentNodeChanged(vtkMRMLNode*) ), this, SLOT( doseVolumeNodeChanged(vtkMRMLNode*) ) );
  connect( d->spinBox_NumberOfLevels, SIGNAL(valueChanged(int)), this, SLOT(setNumberOfLevels(int)));

  connect( d->checkBox_ShowDoseVolumesOnly, SIGNAL( stateChanged(int) ), this, SLOT( showDoseVolumesOnlyCheckboxChanged(int) ) );
  connect( d->checkBox_Isoline, SIGNAL(toggled(bool)), this, SLOT( setIsolineVisibility(bool) ) );
  connect( d->checkBox_Isosurface, SIGNAL(toggled(bool)), this, SLOT( setIsosurfaceVisibility(bool) ) );
  connect( d->checkBox_ScalarBar, SIGNAL(toggled(bool)), this, SLOT( setScalarBarVisibility(bool) ) );
  connect( d->checkBox_ScalarBar2D, SIGNAL(toggled(bool)), this, SLOT( setScalarBar2DVisibility(bool) ) );

  connect( d->pushButton_Apply, SIGNAL(clicked()), this, SLOT(applyClicked()) );

  d->pushButton_Apply->setMinimumSize(d->pushButton_Apply->sizeHint().width() + 8, d->pushButton_Apply->sizeHint().height() + 4);

  qSlicerApplication * app = qSlicerApplication::application();
  if (app && app->layoutManager())
  {
    qMRMLThreeDView* threeDView = app->layoutManager()->threeDWidget(0)->threeDView();
    vtkRenderer* activeRenderer = app->layoutManager()->activeThreeDRenderer();
    if (activeRenderer)
    {
      d->ScalarBarWidget->SetInteractor(activeRenderer->GetRenderWindow()->GetInteractor());
    }
    connect(d->checkBox_ScalarBar, SIGNAL(stateChanged(int)), threeDView, SLOT(scheduleRender()));

    QStringList sliceViewerNames = app->layoutManager()->sliceViewNames();
    qMRMLSliceWidget* sliceViewerWidgetRed = app->layoutManager()->sliceWidget(sliceViewerNames[0]);
    const qMRMLSliceView* sliceViewRed = sliceViewerWidgetRed->sliceView();
    d->ScalarBarWidget2DRed->SetInteractor(sliceViewerWidgetRed->interactorStyle()->GetInteractor());
    qMRMLSliceWidget* sliceViewerWidgetYellow = app->layoutManager()->sliceWidget(sliceViewerNames[1]);
    const qMRMLSliceView* sliceViewYellow = sliceViewerWidgetYellow->sliceView();
    d->ScalarBarWidget2DYellow->SetInteractor(sliceViewerWidgetYellow->interactorStyle()->GetInteractor());
    qMRMLSliceWidget* sliceViewerWidgetGreen = app->layoutManager()->sliceWidget(sliceViewerNames[2]);
    const qMRMLSliceView* sliceViewGreen = sliceViewerWidgetGreen->sliceView();
    d->ScalarBarWidget2DGreen->SetInteractor(sliceViewerWidgetGreen->interactorStyle()->GetInteractor());

    connect(d->checkBox_ScalarBar2D, SIGNAL(stateChanged(int)), sliceViewRed, SLOT(scheduleRender()));
    connect(d->checkBox_ScalarBar2D, SIGNAL(stateChanged(int)), sliceViewYellow, SLOT(scheduleRender()));
    connect(d->checkBox_ScalarBar2D, SIGNAL(stateChanged(int)), sliceViewGreen, SLOT(scheduleRender()));
  }

  // Handle scene change event if occurs
  qvtkConnect( d->logic(), vtkCommand::ModifiedEvent, this, SLOT( onLogicModified() ) );

  // Select the default color node
  this->updateScalarBarsFromSelectedColorTable();

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::setParameterNode(vtkMRMLNode *node)
{
  Q_D(qSlicerIsodoseModuleWidget);

  if (!this->mrmlScene())
  {
    qCritical() << Q_FUNC_INFO << ": Invalid scene";
    return;
  }
  
  vtkMRMLIsodoseNode* paramNode = vtkMRMLIsodoseNode::SafeDownCast(node);

  // Make sure the parameter set node is selected (in case the function was not called by the selector combobox signal)
  d->MRMLNodeComboBox_ParameterSet->setCurrentNode(paramNode);

  // Each time the node is modified, the qt widgets are updated
  qvtkReconnect( paramNode, vtkCommand::ModifiedEvent, this, SLOT(updateWidgetFromMRML()) );

  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::doseVolumeNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerIsodoseModuleWidget);

  if (!this->mrmlScene())
  {
    qCritical() << Q_FUNC_INFO << ": Invalid scene";
    return;
  }

  vtkMRMLIsodoseNode* paramNode = vtkMRMLIsodoseNode::SafeDownCast(d->MRMLNodeComboBox_ParameterSet->currentNode());
  if (!paramNode || !node)
  {
    return;
  }

  // Unobserve previous color node
  vtkMRMLColorTableNode* previousColorNode = paramNode->GetColorTableNode();
  if (previousColorNode)
  {
    qvtkDisconnect(previousColorNode, vtkCommand::ModifiedEvent,
      this, SLOT(updateScalarBarsFromSelectedColorTable()));
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetAndObserveDoseVolumeNode(vtkMRMLScalarVolumeNode::SafeDownCast(node));
  paramNode->DisableModifiedEventOff();

  if (paramNode->GetDoseVolumeNode() && vtkSlicerRtCommon::IsDoseVolumeNode(paramNode->GetDoseVolumeNode()))
  {
    d->label_NotDoseVolumeWarning->setText("");
  }
  else
  {
    d->label_NotDoseVolumeWarning->setText(tr(" Selected volume is not a dose"));
  }

  // Make sure the dose volume has an associated isodose color table node
  d->logic()->SetupColorTableNodeForDoseVolumeNode(vtkMRMLScalarVolumeNode::SafeDownCast(node));
  // Show color table node associated to the dose volume
  vtkMRMLColorTableNode* selectedColorNode = paramNode->GetColorTableNode();
  d->tableView_IsodoseLevels->setMRMLColorNode(selectedColorNode);
  // Set current number of isodose levels
  bool wasBlocked = d->spinBox_NumberOfLevels->blockSignals(true);
  if (selectedColorNode)
  {
    d->spinBox_NumberOfLevels->setValue(selectedColorNode->GetNumberOfColors());

    qvtkConnect(selectedColorNode, vtkCommand::ModifiedEvent,
      this, SLOT(updateScalarBarsFromSelectedColorTable()));
  }
  else
  {
    d->spinBox_NumberOfLevels->setValue(0);
  }
  d->spinBox_NumberOfLevels->blockSignals(wasBlocked);
  // Update scalar bars
  this->updateScalarBarsFromSelectedColorTable();

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::setNumberOfLevels(int newNumber)
{
  Q_D(qSlicerIsodoseModuleWidget);

  //if (!d->spinBox_NumberOfLevels->isEnabled()) //TODO: Needed?
  //{
  //  return;
  //}

  vtkMRMLIsodoseNode* paramNode = vtkMRMLIsodoseNode::SafeDownCast(d->MRMLNodeComboBox_ParameterSet->currentNode());
  if (!paramNode)
  {
    return;
  }

  d->logic()->SetNumberOfIsodoseLevels(paramNode, newNumber);
  vtkMRMLColorTableNode* selectedColorNode = paramNode->GetColorTableNode();
  if (!selectedColorNode)
  {
    qCritical() << Q_FUNC_INFO << ": Invalid color table node";
    return;
  }

  int numberOfColors = selectedColorNode->GetNumberOfColors();
  d->ScalarBarActor->SetMaximumNumberOfColors(numberOfColors);
  d->ScalarBarActor->SetNumberOfLabels(numberOfColors);

  d->ScalarBarActor2DRed->SetMaximumNumberOfColors(numberOfColors);
  d->ScalarBarActor2DRed->SetNumberOfLabels(numberOfColors);
  d->ScalarBarActor2DYellow->SetMaximumNumberOfColors(numberOfColors);
  d->ScalarBarActor2DYellow->SetNumberOfLabels(numberOfColors);
  d->ScalarBarActor2DGreen->SetMaximumNumberOfColors(numberOfColors);
  d->ScalarBarActor2DGreen->SetNumberOfLabels(numberOfColors);

  this->updateScalarBarsFromSelectedColorTable();
}

//-----------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::showDoseVolumesOnlyCheckboxChanged(int aState)
{
  Q_D(qSlicerIsodoseModuleWidget);
  
  if (!this->mrmlScene())
  {
    qCritical() << Q_FUNC_INFO << ": Invalid scene";
    return;
  }
  
  vtkMRMLIsodoseNode* paramNode = vtkMRMLIsodoseNode::SafeDownCast(d->MRMLNodeComboBox_ParameterSet->currentNode());
  if (!paramNode)
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetShowDoseVolumesOnly(aState);
  paramNode->DisableModifiedEventOff();

  if (aState)
  {
    d->MRMLNodeComboBox_DoseVolume->addAttribute("vtkMRMLScalarVolumeNode", vtkSlicerRtCommon::DICOMRTIMPORT_DOSE_VOLUME_IDENTIFIER_ATTRIBUTE_NAME.c_str());
  }
  else
  {
    d->MRMLNodeComboBox_DoseVolume->removeAttribute("vtkMRMLScalarVolumeNode", vtkSlicerRtCommon::DICOMRTIMPORT_DOSE_VOLUME_IDENTIFIER_ATTRIBUTE_NAME.c_str());
  }
}

//-----------------------------------------------------------------------------
QString qSlicerIsodoseModuleWidget::generateNewIsodoseLevel() const
{
  QString newIsodoseLevelBase("New level");
  QString newIsodoseLevel(newIsodoseLevelBase);
  return newIsodoseLevel;
}

//------------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::setIsolineVisibility(bool visible)
{
  Q_D(qSlicerIsodoseModuleWidget);

  if (!this->mrmlScene())
  {
    qCritical() << Q_FUNC_INFO << ": Invalid scene";
    return;
  }

  vtkMRMLIsodoseNode* paramNode = vtkMRMLIsodoseNode::SafeDownCast(d->MRMLNodeComboBox_ParameterSet->currentNode());
  if (!paramNode)
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetShowIsodoseLines(visible);
  paramNode->DisableModifiedEventOff();

  vtkMRMLModelHierarchyNode* modelHierarchyNode = d->logic()->GetRootModelHierarchyNode(paramNode);
  if(!modelHierarchyNode)
  {
    qCritical() << Q_FUNC_INFO << ": Invalid isodose surface models parent hierarchy node";
    return;
  }

  vtkSmartPointer<vtkCollection> childModelNodes = vtkSmartPointer<vtkCollection>::New();
  modelHierarchyNode->GetChildrenModelNodes(childModelNodes);
  childModelNodes->InitTraversal();
  for (int i=0; i<childModelNodes->GetNumberOfItems(); ++i)
  {
    vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(childModelNodes->GetItemAsObject(i));
    modelNode->GetDisplayNode()->SetSliceIntersectionVisibility(visible);
  }
}

//------------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::setIsosurfaceVisibility(bool visible)
{
  Q_D(qSlicerIsodoseModuleWidget);

  if (!this->mrmlScene())
  {
    qCritical() << Q_FUNC_INFO << ": Invalid scene";
    return;
  }

  vtkMRMLIsodoseNode* paramNode = vtkMRMLIsodoseNode::SafeDownCast(d->MRMLNodeComboBox_ParameterSet->currentNode());
  if (!paramNode)
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetShowIsodoseSurfaces(visible);
  paramNode->DisableModifiedEventOff();

  vtkMRMLModelHierarchyNode* modelHierarchyNode = d->logic()->GetRootModelHierarchyNode(paramNode);
  if(!modelHierarchyNode)
  {
    return;
  }

  vtkSmartPointer<vtkCollection> childModelNodes = vtkSmartPointer<vtkCollection>::New();
  modelHierarchyNode->GetChildrenModelNodes(childModelNodes);
  childModelNodes->InitTraversal();
  for (int i=0; i<childModelNodes->GetNumberOfItems(); ++i)
  {
    vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(childModelNodes->GetItemAsObject(i));
    modelNode->GetDisplayNode()->SetVisibility(visible);
  }
}

//------------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::setScalarBarVisibility(bool visible)
{
  Q_D(qSlicerIsodoseModuleWidget);

  if (!this->mrmlScene())
  {
    qCritical() << Q_FUNC_INFO << ": Invalid scene";
    return;
  }

  vtkMRMLIsodoseNode* paramNode = vtkMRMLIsodoseNode::SafeDownCast(d->MRMLNodeComboBox_ParameterSet->currentNode());
  if (!paramNode)
  {
    return;
  }
  if (!d->ScalarBarWidget)
  {
    qCritical() << Q_FUNC_INFO << ": Invalid scalar bar widget";
    return;
  }
  if (visible) //TODO:
  {
    d->ScalarBarActor->UseAnnotationAsLabelOn();
  }

  vtkMRMLColorTableNode* selectedColorNode = paramNode->GetColorTableNode();
  if (!selectedColorNode)
  {
    qCritical() << Q_FUNC_INFO << ": Invalid color table node";
    return;
  }

  this->updateScalarBarsFromSelectedColorTable();

  d->ScalarBarWidget->SetEnabled(visible);
}

//------------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::setScalarBar2DVisibility(bool visible)
{
  Q_D(qSlicerIsodoseModuleWidget);

  if (!this->mrmlScene())
  {
    qCritical() << Q_FUNC_INFO << ": Invalid scene";
    return;
  }

  vtkMRMLIsodoseNode* paramNode = vtkMRMLIsodoseNode::SafeDownCast(d->MRMLNodeComboBox_ParameterSet->currentNode());
  if (!paramNode)
  {
    return;
  }
  if (!d->ScalarBarWidget2DRed || !d->ScalarBarWidget2DYellow || !d->ScalarBarWidget2DGreen)
  {
    qCritical() << Q_FUNC_INFO << ": Invalid scalar bar widget";
    return;
  }
  if (visible) //TODO:
  {
    d->ScalarBarActor2DRed->UseAnnotationAsLabelOn();
    d->ScalarBarActor2DYellow->UseAnnotationAsLabelOn();
    d->ScalarBarActor2DGreen->UseAnnotationAsLabelOn();
  }

  vtkMRMLColorTableNode* selectedColorNode = paramNode->GetColorTableNode();
  if (!selectedColorNode)
  {
    qCritical() << Q_FUNC_INFO << ": Invalid color table node";
    return;
  }

  this->updateScalarBarsFromSelectedColorTable();

  d->ScalarBarWidget2DRed->SetEnabled(visible);
  d->ScalarBarWidget2DYellow->SetEnabled(visible);
  d->ScalarBarWidget2DGreen->SetEnabled(visible);
}

//-----------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::applyClicked()
{
  Q_D(qSlicerIsodoseModuleWidget);

  if (!this->mrmlScene())
  {
    qCritical() << Q_FUNC_INFO << ": Invalid scene";
    return;
  }

  vtkMRMLIsodoseNode* paramNode = vtkMRMLIsodoseNode::SafeDownCast(d->MRMLNodeComboBox_ParameterSet->currentNode());
  if (!paramNode)
  {
    return;
  }

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // Compute the isodose surface for the selected dose volume
  d->logic()->CreateIsodoseSurfaces(paramNode);

  // Add new model hierarchy under dose
  // (cannot be done in the logic because automatic tests do not have automatic SH creation so there
  // will be no SH item for the root model hierarchy node)
  vtkMRMLSubjectHierarchyNode* shNode = vtkMRMLSubjectHierarchyNode::GetSubjectHierarchyNode(this->mrmlScene());
  vtkIdType doseShItemID = shNode->GetItemByDataNode(paramNode->GetDoseVolumeNode());
  vtkMRMLModelHierarchyNode* rootModelHierarchyNode = d->logic()->GetRootModelHierarchyNode(paramNode);
  if (!doseShItemID || !rootModelHierarchyNode)
  {
    qWarning() << Q_FUNC_INFO << ": No root model hierarchy node. Creating isodose surfaces may have failed";
  }
  else
  {
    shNode->SetItemParent(shNode->GetItemByDataNode(rootModelHierarchyNode), doseShItemID);
  }

  QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::updateButtonsState()
{
  Q_D(qSlicerIsodoseModuleWidget);

  vtkMRMLIsodoseNode* paramNode = vtkMRMLIsodoseNode::SafeDownCast(d->MRMLNodeComboBox_ParameterSet->currentNode());
  bool applyEnabled = paramNode
                   && paramNode->GetDoseVolumeNode()
                   && paramNode->GetColorTableNode()
                   && paramNode->GetColorTableNode()->GetNumberOfColors() > 0;
  d->pushButton_Apply->setEnabled(applyEnabled);
}

//-----------------------------------------------------------
bool qSlicerIsodoseModuleWidget::setEditedNode(
  vtkMRMLNode* node, QString role /* = QString()*/, QString context /* = QString() */)
{
  Q_D(qSlicerIsodoseModuleWidget);
  Q_UNUSED(role);
  Q_UNUSED(context);

  if (!vtkSlicerRtCommon::IsDoseVolumeNode(node))
    {
    return false;
    }

  d->MRMLNodeComboBox_DoseVolume->setCurrentNode(node);
  return true;
}

//-----------------------------------------------------------------------------
void qSlicerIsodoseModuleWidget::updateScalarBarsFromSelectedColorTable()
{
  Q_D(qSlicerIsodoseModuleWidget);

  vtkMRMLIsodoseNode* paramNode = vtkMRMLIsodoseNode::SafeDownCast(d->MRMLNodeComboBox_ParameterSet->currentNode());
  if (!this->mrmlScene() || !paramNode)
  {
    return;
  }

  vtkMRMLColorTableNode* selectedColorNode = paramNode->GetColorTableNode();
  if (!selectedColorNode)
  {
    qDebug() << Q_FUNC_INFO << ": No color table node is selected";
    return;
  }

  int newNumberOfColors = selectedColorNode->GetNumberOfColors();

  // Update all scalar bar actors
  for (std::vector<vtkScalarBarWidget*>::iterator it = d->ScalarBarWidgets.begin();
    it != d->ScalarBarWidgets.end(); ++it)
  {
    vtkSlicerRTScalarBarActor* actor = vtkSlicerRTScalarBarActor::SafeDownCast(
      (*it)->GetScalarBarActor() );

    // Update actor
    actor->UseAnnotationAsLabelOn(); // Needed each time
    actor->SetLookupTable(selectedColorNode->GetLookupTable());
    actor->SetNumberOfLabels(newNumberOfColors);
    actor->SetMaximumNumberOfColors(newNumberOfColors);
    actor->GetLookupTable()->ResetAnnotations();
    for (int colorIndex=0; colorIndex<newNumberOfColors; ++colorIndex)
    {
      actor->GetLookupTable()->SetAnnotation(colorIndex, vtkStdString(selectedColorNode->GetColorName(colorIndex)));
    }
    (*it)->Render();
  }
}
