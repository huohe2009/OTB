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
#ifndef __otbWrapperInputImageListParameter_h
#define __otbWrapperInputImageListParameter_h

#include "otbImageFileReader.h"

#include "otbWrapperParameter.h"
#include "otbObjectList.h"

namespace otb
{
namespace Wrapper
{
/** \class InputImageListParameter
 *  \brief This class represents a list of InputImage parameter
 */

class ITK_EXPORT InputImageListParameter : public Parameter
{
public:
  /** Standard class typedef */
  typedef InputImageListParameter           Self;
  typedef Parameter                     Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  typedef otb::ImageFileReader<FloatVectorImageType> ImageFileReaderType;
  typedef otb::ObjectList<ImageFileReaderType>  ImageFileReaderListType;

  /** Defining ::New() static method */
  itkNewMacro(Self);

  /** RTTI support */
  itkTypeMacro(InputImageListParameter, Parameter);

  /** Set image form a list of filename */
  void SetListFromFileName(const std::vector<std::string> & filenames)
  {
    for(unsigned int i=0; i<filenames.size(); i++)
      {
        ImageFileReaderType::Pointer reader = ImageFileReaderType::New();
        reader->SetFileName(filenames[i]);
        reader->UpdateOutputInformation();
        
        // everything went fine, store the object references
        m_ReaderList->PushBack(reader);
        m_ImageList->PushBack(reader->GetOutput());
      }
    SetActive(true);

    this->Modified();
  }

  /** Add an image from a filename */
  void AddFromFileName(const std::string & filename)
  {
    ImageFileReaderType::Pointer reader = ImageFileReaderType::New();
    reader->SetFileName(filename);
    reader->UpdateOutputInformation();
        
    // everything went fine, store the object references
    m_ReaderList->PushBack(reader);
    m_ImageList->PushBack(reader->GetOutput());
    SetActive(true);
    this->Modified();
  }

 /** Set one specific stored image filename. */
  void SetNthFileName( const unsigned int id, const std::string & filename )
  {
    if( m_ReaderList->Size()<id )
      {
      itkExceptionMacro(<< "No image "<<id<<". Only "<<m_ReaderList->Size()<<" images available.");
      }
    
    ImageFileReaderType::Pointer reader = ImageFileReaderType::New();
    reader->SetFileName(filename);
    reader->UpdateOutputInformation();
    m_ReaderList->SetNthElement(id, reader);
    m_ImageList->SetNthElement(id, reader->GetOutput());
  }


  /** Get the stored image filename list */
  std::vector<std::string> GetFileNameList() const
  {
    if (m_ReaderList)
      {
        std::vector<std::string> filenames;
        for(unsigned int i=0; i<m_ReaderList->Size(); i++)
          {
            filenames.push_back( m_ReaderList->GetNthElement(i)->GetFileName() );
          }

      return filenames;
      }

    itkExceptionMacro(<< "No filename value");
  }

 /** Get one specific stored image filename. */
  std::string GetNthFileName( unsigned int i ) const
  {
    if (m_ReaderList)
      {
        if(m_ReaderList->Size()<i)
          {
            itkExceptionMacro(<< "No image "<<i<<". Only "<<m_ReaderList->Size()<<" images available.");
          }

        return m_ReaderList->GetNthElement(i)->GetFileName();
      }

    itkExceptionMacro(<< "No filename value");
  }

  /** Get one list of the stored image. */
  FloatVectorImageListType* GetImageList() const
  {
    return m_ImageList;
  }

  /** Get one specific stored image. */
  FloatVectorImageType* GetNthImage(unsigned int i) const
  {
    if(m_ImageList->Size()<i)
      {
        itkExceptionMacro(<< "No image "<<i<<". Only "<<m_ImageList->Size()<<" images available.");
      }
    return m_ImageList->GetNthElement(i);
  }

  /** Set the list of image. */
  void SetImageList(FloatVectorImageListType* imList)
  {
    m_ImageList = imList;
    m_ReaderList = ImageFileReaderListType::Pointer();
    for(unsigned int i=0; i<m_ImageList->Size(); i++)
      {
        m_ReaderList->PushBack( ImageFileReaderType::Pointer() );
      }

    this->Modified();
  }

  /** Add an image to the list. */
  void AddImage(FloatVectorImageType* image)
  {
    m_ImageList->PushBack( image );
    m_ReaderList->PushBack( ImageFileReaderType::Pointer() );
  }

  bool HasValue() const
  {
    if(m_ImageList->Size() == 0)
      {
      return false;
      }

    return m_ImageList->GetNthElement(0).IsNotNull();
  }

  void Erase( unsigned int id )
  {
    if(m_ImageList->Size()<id)
      {
      itkExceptionMacro(<< "No image "<<id<<". Only "<<m_ImageList->Size()<<" images available.");
      }
    
    m_ImageList->Erase( id );
    m_ReaderList->Erase( id );
  }

  void ClearValue()
  {
    m_ImageList = FloatVectorImageListType::Pointer();
    m_ReaderList = ImageFileReaderListType::Pointer();
  }

  void SwitchOrder( std::map<unisgned int, unsigned int> idMap )
  {
    if(idMap.size() != m_ImageList->Size())
      {
      itkExceptionMacro(<< "Map size and the number of image mismatches ("<<idMap.size()<<" vs. "<<m_ImageList->Size()<<").");
      }

    FloatVectorImageListType::Pointer tmpImageList = FloatVectorImageListType::New();
    ImageFileReaderListType::Pointer  tmpReaderList = ImageFileReaderListType::New();

    for( unsigned int i=0; i<idMap.size(); i++)
      {
      tmpImageList->PushBack( m_ImageList->GetNthElement(idMap[i]) );
      tmpReaderList->PushBack( m_ReaderList->GetNthElement(idMap[i]) );

      m_ReaderList = tmpReaderList;
      m_ImageList = tmpImageList;
      }
  }

protected:
  /** Constructor */
  InputImageListParameter()
  {
    this->SetName("Input Image List");
    this->SetKey("inList");
    m_ImageList = FloatVectorImageListType::New();
    m_ReaderList = ImageFileReaderListType::New();
  }

  /** Destructor */
  virtual ~InputImageListParameter()
  {}

 
  FloatVectorImageListType::Pointer m_ImageList;
  ImageFileReaderListType::Pointer  m_ReaderList;

private:
  InputImageListParameter(const Parameter &); //purposely not implemented
  void operator =(const Parameter&); //purposely not implemented

}; // End class InputImage Parameter

} // End namespace Wrapper
} // End namespace otb

#endif
