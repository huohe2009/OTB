/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbBinaryDecisionTreeClassifier_txx
#define __otbBinaryDecisionTreeClassifier_txx

#include "otbBinaryDecisionTreeClassifier.h"
#include "otbMacro.h"

namespace otb
{

template<class TSample, class TLabel>
BinaryDecisionTreeClassifier<TSample, TLabel>
::BinaryDecisionTreeClassifier()
{
  m_Output = OutputType::New();
}

template<class TSample, class TLabel>
void
BinaryDecisionTreeClassifier<TSample, TLabel>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

template<class TSample, class TLabel>
void
BinaryDecisionTreeClassifier<TSample, TLabel>
::Update()
{
  this->GenerateData();
}

template<class TSample, class TLabel>
void
BinaryDecisionTreeClassifier<TSample, TLabel>
::GenerateData()
{
  if (!m_Tree)
    {
    itkExceptionMacro("No tree, can not do classification.");
    }


  m_Output->SetSample(this->GetSample());
  m_Output->Resize(this->GetSample()->Size());

  unsigned int numberOfClasses = this->GetNumberOfClasses();

  m_Output->SetNumberOfClasses(numberOfClasses);

  this->DoClassification();
}

template<class TSample, class TLabel>
typename BinaryDecisionTreeClassifier<TSample, TLabel>::OutputType*
BinaryDecisionTreeClassifier<TSample, TLabel>
::GetOutput()
{
  return m_Output;
}

template<class TSample, class TLabel>
void
BinaryDecisionTreeClassifier<TSample, TLabel>
::SetOutput(OutputType * output)
{
  m_Output = output;
}

template<class TSample, class TLabel>
void
BinaryDecisionTreeClassifier<TSample, TLabel>
::DoClassification()
{
  itk::TimeProbe probe;

  typename TSample::ConstIterator iter = this->GetSample()->Begin();
  typename TSample::ConstIterator end  = this->GetSample()->End();

  typename OutputType::ConstIterator      iterO = m_Output->Begin();
  typename OutputType::ConstIterator      endO  = m_Output->End();
  typename TSample::MeasurementVectorType measurements;

  int numberOfComponentsPerSample  = iter.GetMeasurementVector().Size();

  otbMsgDevMacro(<< "Starting iterations ");
  while (iter != end && iterO != endO)
    {
    int                                    i = 0;
    typename BinaryDecisionTreeModelType::MeasurementType modelMeasurement;

    measurements = iter.GetMeasurementVector();
    // otbMsgDevMacro(  << "Loop on components " << svm_type );
    for (i = 0; i < numberOfComponentsPerSample; ++i)
      {
      modelMeasurement.push_back(measurements[i]);
      }

    ClassLabelType classLabel = m_Model->EvaluateLabel(modelMeasurement);
    // Julien: Event if we support larger type for class labels,
    // the AddInstance method wait for an unsigned int, so we cast it here.
    m_Output->AddInstance(static_cast<unsigned int>(classLabel), iterO.GetInstanceIdentifier());

    ++iter;
    ++iterO;
    }
}

} // end of namespace otb

#endif
