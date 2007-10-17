/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBioCellularAggregate.txx,v $
  Language:  C++
  Date:      $Date: 2007/04/20 13:36:35 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkBioCellularAggregate_txx
#define __itkBioCellularAggregate_txx

#include "itkBioCellularAggregate.h"

namespace itk {

namespace bio {

template<unsigned int NSpaceDimension>
CellularAggregate<NSpaceDimension>
::CellularAggregate()
{
  typename BioCellType::ColorType color;
  color.SetRed(1.0);
  color.SetGreen(1.0);
  color.SetBlue(1.0);
  BioCellType::SetDefaultColor( color );

  m_Mesh = MeshType::New();

  m_Mesh->SetCellsAllocationMethod( MeshType::CellsAllocatedDynamicallyCellByCell );
  m_Mesh->SetPoints( PointsContainer::New() );
  m_Mesh->SetPointData( PointDataContainer::New() );
  m_Mesh->SetCells( VoronoiRegionsContainer::New() );

  m_Iteration = 0;
  m_ClosestPointComputationInterval = 5;

  m_FrictionForce = 1.0f;

}

template<unsigned int NSpaceDimension>
CellularAggregate<NSpaceDimension>
::~CellularAggregate()
{
  m_Mesh->DebugOn();
  this->KillAll();
}

template<unsigned int NSpaceDimension>
unsigned int
CellularAggregate<NSpaceDimension>
::GetNumberOfCells(void) const
{
  return m_Mesh->GetPointData()->Size();
}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::SetGrowthRadiusLimit( double value ) 
{
  BioCellType::SetGrowthRadiusLimit( value );
}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::SetGrowthRadiusIncrement( double value ) 
{
  BioCellType::SetGrowthRadiusIncrement( value );
}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << "Cellular aggregate " << m_Mesh << std::endl;

}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::Remove( CellBase * cellbase )
{
  BioCellType * cell = dynamic_cast<BioCellType *>( cellbase );
  if( !cell )
    {
    itk::ExceptionObject exception;
    exception.SetDescription("Trying to remove a null pointer to cell");
    exception.SetLocation("CellularAggregate::Remove(BioCellType*)");
    throw exception;
    }
  
  IdentifierType id = cell->GetSelfIdentifier();

  typename MeshType::CellAutoPointer   region;
  bool regionExist = m_Mesh->GetCell( id, region );
  if( regionExist )
    {
    VoronoiRegionType * realRegion =
              dynamic_cast < VoronoiRegionType * >( region.GetPointer() );

    if( !realRegion )
      {
      std::cerr << "CellularAggregate::Remove() couldn't dynamic_cast region " << std::endl;
      }
    else
      {
      typename VoronoiRegionType::PointIdIterator neighbor = realRegion->PointIdsBegin();
      typename VoronoiRegionType::PointIdIterator end      = realRegion->PointIdsEnd();
      while( neighbor != end )
        {
        const IdentifierType neighborId = (*neighbor);
        typename MeshType::CellAutoPointer cellPointer;
        bool neighborVoronoiExist = m_Mesh->GetCell( neighborId, cellPointer );
        if( neighborVoronoiExist ) 
          {
          VoronoiRegionType * region =
             dynamic_cast < VoronoiRegionType * >( cellPointer.GetPointer() );
          if( !region )
            {
            std::cerr << "CellularAggregate::Add() Failed to find a region"  << std::endl;
            }
          else
            {
            region->RemovePointId( id );
            }
          }
        neighbor++;
        }
      }
    // update voronoi connections
    }
 
  m_Mesh->GetCells()->DeleteIndex( id );
  m_Mesh->GetPoints()->DeleteIndex( id );
  m_Mesh->GetPointData()->DeleteIndex( id );
 
  delete cell;
}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::GetVoronoi( unsigned long int cellId, VoronoiRegionAutoPointer & voronoiPointer ) const
{
  
  typename MeshType::CellAutoPointer cellPointer;

  bool voronoiExists = m_Mesh->GetCell( cellId, cellPointer );
  if( !voronoiExists ) 
    {
    itk::ExceptionObject exception;
    exception.SetDescription( "voronoi region does not exist in the container" );
    exception.SetLocation( "GetVoronoi( IdentifierType");
    throw exception;
    }

  VoronoiRegionType * region =
          dynamic_cast < VoronoiRegionType * >( cellPointer.GetPointer() );

  voronoiPointer.TakeNoOwnership( region );
  if( cellPointer.IsOwner() )
    {
    voronoiPointer.TakeOwnership();   
    cellPointer.ReleaseOwnership();  
    }

}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::SetEgg( BioCellType * cell, const PointType & position )
{
  VectorType perturbation = position.GetVectorFromOrigin();
  this->Add( cell, perturbation );
}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::Add( CellBase * cell )
{
  VectorType perturbation;
  perturbation.Fill( 0.0 );
  this->Add( cell, perturbation );
}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::Add( CellBase * cellA, CellBase * cellB, double perturbationLength )
{
  // Create a perturbation for separating the daugther cells
  typename BioCellType::VectorType perturbationVector;
  for(unsigned int d=0; d<NSpaceDimension; d++)
    {
    perturbationVector[d] = vnl_sample_uniform( -1.0f, 1.0f ); 
    }

  const double norm = perturbationVector.GetNorm();
  if( vnl_math_abs( norm ) > 1e-10 ) 
    {
    perturbationVector *= perturbationLength / norm;
    }
  else
    {
    // this event should rarely happen... very rarely
    perturbationVector[0] = perturbationLength;
    }

  this->Add( cellA,  perturbationVector );
  this->Add( cellB, -perturbationVector );

  const IdentifierType cellAId = cellA->GetSelfIdentifier();
  const IdentifierType cellBId = cellB->GetSelfIdentifier();

  VoronoiRegionAutoPointer cellAptr;
  VoronoiRegionAutoPointer cellBptr;

  this->GetVoronoi( cellAId, cellAptr );
  this->GetVoronoi( cellBId, cellBptr );

  cellAptr->AddPointId( cellBId );
  cellBptr->AddPointId( cellAId );
}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::Add( CellBase * cellBase, const VectorType & perturbation )
{
  BioCellType * cell = dynamic_cast<BioCellType *>( cellBase );
  IdentifierType  newcellId       = cell->GetSelfIdentifier();
  IdentifierType  newcellparentId = cell->GetParentIdentifier();
    
  PointType position;
  CellAutoPointer selfVoronoi;

  // If the cell does not have a parent 
  // from which receive a position
  if( !newcellparentId )
    {
    position.Fill( 0.0 );
    selfVoronoi.TakeOwnership( new VoronoiRegionType );
    }
  else
    {
    bool parentPositionExist = m_Mesh->GetPoint( newcellparentId, &position );
    if( !parentPositionExist ) 
      {
      itk::ExceptionObject exception;
      exception.SetDescription( "Parent cell does not exist in the container" );
      exception.SetLocation( "CellularAggregate::Add( cell * )" );
      throw exception;
      }

    VoronoiRegionAutoPointer parentVoronoi;
    this->GetVoronoi( newcellparentId, parentVoronoi );
    parentVoronoi->MakeCopy( selfVoronoi );
    }

  position += perturbation;

  m_Mesh->SetCell(      newcellId, selfVoronoi );
  m_Mesh->SetPoint(     newcellId, position    );
  m_Mesh->SetPointData( newcellId, cell        );
 
  cell->SetCellularAggregate( this );

  // Add this new cell as neighbor to cells in its neighborhood

  typename VoronoiRegionType::PointIdIterator neighbor = selfVoronoi->PointIdsBegin();
  typename VoronoiRegionType::PointIdIterator end      = selfVoronoi->PointIdsEnd();

  while( neighbor != end )
    {
    const IdentifierType neighborId = (*neighbor);
    typename MeshType::CellAutoPointer cellPointer;
    bool neighborVoronoiExist = m_Mesh->GetCell( neighborId, cellPointer );
    if( neighborVoronoiExist ) 
      {
      VoronoiRegionType * region =
         dynamic_cast < VoronoiRegionType * >( cellPointer.GetPointer() );

      if( !region )
        {
        std::cerr << "CellularAggregate::Add() Failed to find a region"  << std::endl;
        }
      else
        {
        region->AddPointId( newcellId );
        }
      }
    neighbor++;
    }

}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::AdvanceTimeStep(void)
{
  if( m_Iteration % m_ClosestPointComputationInterval == 0 ) 
  {
    this->ComputeClosestPoints();
  }

  this->ComputeForces();
  this->UpdatePositions();
  
  CellsIterator begin = m_Mesh->GetPointData()->Begin();
  CellsIterator end   = m_Mesh->GetPointData()->End();

  CellsIterator cell  = begin;

  while( cell != end )
    {
    BioCellType * theCell = cell.Value();
    theCell->AdvanceTimeStep();
    ++cell;
    if( theCell->MarkedForRemoval() )
      {
      this->Remove( theCell );
      }
    }
  
  this->InvokeEvent( IterationEvent() );

  m_Iteration++;

}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::KillAll(void)
{
  if( !m_Mesh  )
    {
    return;
    }
  
  CellsIterator cell = m_Mesh->GetPointData()->Begin();
  CellsIterator end  = m_Mesh->GetPointData()->End();

  while( cell != end )
    {
    delete (cell.Value());
    ++cell;
    }

  BioCellType::ResetCounter();
}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::ClearForces(void)
{

  CellsIterator cell = m_Mesh->GetPointData()->Begin();
  CellsIterator end  = m_Mesh->GetPointData()->End();

  while( cell != end )
    {
    cell.Value()->ClearForce();
    ++cell;
    }
}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::UpdatePositions(void)
{
  CellsConstIterator cellIt = m_Mesh->GetPointData()->Begin();
  CellsConstIterator end    = m_Mesh->GetPointData()->End();

  PointType position;

  while( cellIt != end )
    {
    BioCellType * cell = cellIt.Value();
    IdentifierType cellId = cell->GetSelfIdentifier();
    m_Mesh->GetPoint( cellId, &position );
    const VectorType force = cell->GetForce(); 
    if( force.GetNorm() > m_FrictionForce )
      {
      position += force / 50.0;
      }
    m_Mesh->SetPoint( cellId, position );
    cellIt++;
    }

}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::ComputeForces(void)
{
  // Clear all the force accumulators
  this->ClearForces();

  CellsConstIterator   cell1It     = m_Mesh->GetPointData()->Begin();
  CellsConstIterator   end         = m_Mesh->GetPointData()->End();

  // compute forces 
  while( cell1It != end )
    {

    const IdentifierType cell1Id = cell1It.Index();

    BioCellType     * cell1      =  cell1It.Value();

    PointType  position1;
    m_Mesh->GetPoint( cell1Id, &position1 );

    const double rA          = cell1->GetRadius();
 
    VoronoiRegionAutoPointer voronoiRegion;
    this->GetVoronoi( cell1Id, voronoiRegion );
              
    typename VoronoiRegionType::PointIdIterator neighbor = voronoiRegion->PointIdsBegin();
    typename VoronoiRegionType::PointIdIterator end      = voronoiRegion->PointIdsEnd();

    while( neighbor != end )
      {

      const IdentifierType cell2Id = (*neighbor);  

      BioCellType     * cell2 = 0;
      PointType  position2;

      if( !m_Mesh->GetPoint(      cell2Id, &position2 ) )
        {
        neighbor++;  // if the neigbor has been removed, skip it
        continue;
        }
      m_Mesh->GetPointData(  cell2Id, &cell2     );
      
      const double rB      = cell2->GetRadius();

      typename BioCellType::VectorType relativePosition = position1 - position2;

      const double distance = relativePosition.GetNorm();

      if( distance < (rA + rB) / 2.0 )
        {
        const double factor = 2.0 * BioCellType::GetGrowthRadiusLimit() / distance;
        typename BioCellType::VectorType force = relativePosition * factor;
        cell1->AddForce(  force );
        cell2->AddForce( -force );
        }
      else if( distance < rA + rB )
        {
        typename BioCellType::VectorType force = relativePosition;
        cell1->AddForce(  force );
        cell2->AddForce( -force );
        }
      
      neighbor++;
      }
    
    cell1It++;
    }

}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::ComputeClosestPoints(void)
{

  PointsConstIterator   beginPoints   = m_Mesh->GetPoints()->Begin();
  PointsConstIterator   endPoints     = m_Mesh->GetPoints()->End();

  PointsConstIterator   point1It = beginPoints;

  while( point1It != endPoints )
    {

    PointType  position1 = point1It.Value();

    PointsConstIterator   point2It   = beginPoints;

    BioCellType  * cell1 = 0;

    IdentifierType cell1Id = point1It.Index();
    m_Mesh->GetPointData( cell1Id, &cell1 );
    const double radius  = cell1->GetRadius();
    const double limitDistance = radius * 4.0;
 
    VoronoiRegionAutoPointer voronoiRegion;
    this->GetVoronoi( cell1Id, voronoiRegion ); 

    voronoiRegion->ClearPoints();

    while( point2It != endPoints )
      {

      if( point2It == point1It )
        {
        point2It++;
        continue;
        }

      PointType  position2 = point2It.Value();

      typename BioCellType::VectorType relativePosition = position1 - position2;

      const double distance = relativePosition.GetNorm();
      if( distance < limitDistance )
        {
        voronoiRegion->AddPointId( point2It.Index() );
        }
      point2It++;
      }

    point1It++;
    }

}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::DumpContent( std::ostream & os ) const
{
  CellsConstIterator   beginCell   = m_Mesh->GetPointData()->Begin();
  CellsConstIterator   endCell     = m_Mesh->GetPointData()->End();
  
  CellsConstIterator cell1It = beginCell;

  os << "Cell Identifiers " << std::endl;
  while( cell1It != endCell )
    {
    os << cell1It.Index() << " == ";
    os << cell1It.Value()->GetSelfIdentifier() << std::endl;
    cell1It++;
    }

  os << std::endl << "Points " << std::endl;
  PointsConstIterator  pointIt     = m_Mesh->GetPoints()->Begin();
  PointsConstIterator  endPoint    = m_Mesh->GetPoints()->End();

  while( pointIt != endPoint )
    {
    os << pointIt.Index() << "  ";
    os << pointIt.Value() << std::endl; 
    pointIt++;
    }

  os << std::endl << "Neighborhoods " << std::endl;
  cell1It = beginCell;
  while( cell1It != endCell )
    {
    VoronoiRegionAutoPointer voronoiRegion;
    this->GetVoronoi( cell1It.Index(), voronoiRegion );
    typename VoronoiRegionType::PointIdIterator neighbor = voronoiRegion->PointIdsBegin();
    typename VoronoiRegionType::PointIdIterator end      = voronoiRegion->PointIdsEnd();

    while( neighbor != end )
      {
      os << (*neighbor) << "   ";
      neighbor++;
      }
    os << std::endl;

    cell1It++;
    }
      
}

template<unsigned int NSpaceDimension>
void
CellularAggregate<NSpaceDimension>
::AddSubstrate( SubstrateType * substrate )
{
  SubstratePointer smartPointer( substrate );
  m_Substrates.push_back( smartPointer );
}

template<unsigned int NSpaceDimension>
typename CellularAggregate<NSpaceDimension>::SubstratesVector &
CellularAggregate<NSpaceDimension>
::GetSubstrates( void )
{
  return m_Substrates;
}

template<unsigned int NSpaceDimension>
typename CellularAggregate<NSpaceDimension>::SubstrateValueType 
CellularAggregate<NSpaceDimension>
::GetSubstrateValue( unsigned long int cellId, unsigned int substrateId ) const
{
  PointType cellPosition;
  bool cellPositionExists = m_Mesh->GetPoint( cellId, &cellPosition );
  if( !cellPositionExists ) 
    {
    std::cerr << " Cell position doesn't exist for cell Id = ";
    std::cerr << cellId << std::endl;
    return itk::NumericTraits< SubstrateValueType >::Zero;
    }

  SubstratePointer    substrate = m_Substrates[ substrateId ];

  typename SubstrateType::IndexType index;
  typedef typename SubstrateType::IndexType::IndexValueType IndexValueType;

  substrate->TransformPhysicalPointToIndex( cellPosition, index );

  SubstrateValueType  value = 0;
  if( substrate->GetBufferedRegion().IsInside( index ) )
    {
    value = substrate->GetPixel( index );
    }

  return value;
}

} // end namespace bio

} // end namespace itk

#endif
