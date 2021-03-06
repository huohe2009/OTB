#
# Copyright (C) 2005-2017 Centre National d'Etudes Spatiales (CNES)
#
# This file is part of Orfeo Toolbox
#
#     https://www.orfeo-toolbox.org/
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set(OTBAppClassification_LINK_LIBS
  ${OTBVectorDataBase_LIBRARIES}
  ${OTBConversion_LIBRARIES}
  ${OTBStatistics_LIBRARIES}
  ${OTBColorMap_LIBRARIES}
  ${OTBBoost_LIBRARIES}
  ${OTBInterpolation_LIBRARIES}
  ${OTBMajorityVoting_LIBRARIES}
  ${OTBVectorDataIO_LIBRARIES}
  ${OTBSOM_LIBRARIES}
  ${OTBSVMLearning_LIBRARIES}
  ${OTBLearningBase_LIBRARIES}
  ${OTBApplicationEngine_LIBRARIES}
  ${OTBIndices_LIBRARIES}
  ${OTBMathParser_LIBRARIES}
  ${OTBDempsterShafer_LIBRARIES}
  ${OTBGdalAdapters_LIBRARIES}
  ${OTBProjection_LIBRARIES}
  ${OTBImageBase_LIBRARIES}
  ${OTBIOXML_LIBRARIES}
  ${OTBVectorDataManipulation_LIBRARIES}
  ${OTBStreaming_LIBRARIES}
  ${OTBImageManipulation_LIBRARIES}
  ${OTBObjectList_LIBRARIES}
  ${OTBCommon_LIBRARIES}
)

otb_create_application(
  NAME           ComputeOGRLayersFeaturesStatistics
  SOURCES        otbComputeOGRLayersFeaturesStatistics.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           SOMClassification
  SOURCES        otbSOMClassification.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           ImageClassifier
  SOURCES        otbImageClassifier.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           TrainVectorClassifier
  SOURCES        otbTrainVectorClassifier.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           ComputeConfusionMatrix
  SOURCES        otbComputeConfusionMatrix.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           FusionOfClassifications
  SOURCES        otbFusionOfClassifications.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           ComputePolylineFeatureFromImage
  SOURCES        otbComputePolylineFeatureFromImage.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           KMeansClassification
  SOURCES        otbKMeansClassification.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           TrainImagesClassifier
  SOURCES        otbTrainImagesClassifier.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           TrainRegression
  SOURCES        otbTrainRegression.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           PredictRegression
  SOURCES        otbPredictRegression.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           DSFuzzyModelEstimation
  SOURCES        otbDSFuzzyModelEstimation.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           ClassificationMapRegularization
  SOURCES        otbClassificationMapRegularization.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           OGRLayerClassifier
  SOURCES        otbOGRLayerClassifier.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           ComputeImagesStatistics
  SOURCES        otbComputeImagesStatistics.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           VectorDataDSValidation
  SOURCES        otbVectorDataDSValidation.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           PolygonClassStatistics
  SOURCES        otbPolygonClassStatistics.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           SampleSelection
  SOURCES        otbSampleSelection.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           SampleExtraction
  SOURCES        otbSampleExtraction.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           MultiImageSamplingRate
  SOURCES        otbMultiImageSamplingRate.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           VectorClassifier
  SOURCES        otbVectorClassifier.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})
