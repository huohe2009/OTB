/*
 * Copyright (C) 2005-2017 Centre National d'Etudes Spatiales (CNES)
 *
 * This file is part of Orfeo Toolbox
 *
 *     https://www.orfeo-toolbox.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "itkMacro.h"
#include "otbImageToImageRCC8Calculator.h"
#include "otbImageList.h"
#include "otbImageFileReader.h"

int otbImageToImageRCC8Calculator(int itkNotUsed(argc), char* argv[])
{
  const unsigned int Dimension = 2;

  int    nbImages  = atoi(argv[1]);
  char * outfile   = argv[2];

  typedef unsigned char                              PixelType;
  typedef otb::Image<PixelType, Dimension>           ImageType;
  typedef otb::ImageFileReader<ImageType>            ReaderType;
  typedef otb::ImageToImageRCC8Calculator<ImageType> CalculatorType;
  typedef otb::ImageList<ImageType>                  ImageListType;
  typedef ImageListType::Iterator                    IteratorType;
  // reference image list
  ImageListType::Pointer images = ImageListType::New();

  // Reading input images
  std::ofstream out;
  out.open(outfile, std::ios::out);
  out << "Test results from otbImageToImageRCC8calculator test." << std::endl;
  for (int k = 1; k <= nbImages; ++k)
    {
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(argv[2 + k]);
    reader->Update();
    images->PushBack(reader->GetOutput());
    }
  // Declaration
  CalculatorType::Pointer calc, calc1, calc2;
  // Computing relations for each images couple
  int i = 1;
  int j = 1;
  for (IteratorType it1 = images->Begin(); it1 != images->End(); ++it1)
    {
    for (IteratorType it2 = images->Begin(); it2 != images->End(); ++it2)
      {
      std::cout << "Test: computing relation " << i << "," << j << std::endl;
      calc = CalculatorType::New();
      calc->SetInput1(it1.Get());
      calc->SetInput2(it2.Get());
      calc->Update();
      out << calc->GetValue() << "\t";

      if (calc->GetValue() < 3
          || calc->GetValue() == 4
          || calc->GetValue() == 6)
        {
        calc1 = CalculatorType::New();
        calc1->SetInput1(it1.Get());
        calc1->SetInput2(it2.Get());
        calc1->SetLevel1APrioriKnowledge(true);
        calc1->Update();
        if (calc1->GetValue() != calc->GetValue())
          {
          std::cout << "Test failed: Result with level1AprioriKnowledge ";
          std::cout << "different from result without a priori knowledge" << std::endl;
          std::cout << calc->GetValue() << "!=" << calc1->GetValue() << std::endl;
          return EXIT_FAILURE;
          }
        }
      if (calc->GetValue() < 4)
        {
        calc2 = CalculatorType::New();
        calc2->SetInput1(it1.Get());
        calc2->SetInput2(it2.Get());
        calc2->SetLevel3APrioriKnowledge(true);
        calc2->Update();
        if (calc2->GetValue() != calc->GetValue())
          {
          std::cout << "Test failed: Result with level3AprioriKnowledge ";
          std::cout << "different from result without a priori knowledge" << std::endl;
          std::cout << calc->GetValue() << "!=" << calc1->GetValue() << std::endl;
          return EXIT_FAILURE;
          }
        }
      ++j;
      }
    j = 1;
    ++i;
    out << std::endl;
    }
  out.close();

  return EXIT_SUCCESS;
}
