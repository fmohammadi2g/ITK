/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkQuadEdgeMeshPolygonCell.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkQuadEdgeMeshPolygonCell_h
#define __itkQuadEdgeMeshPolygonCell_h

namespace itk
{

/** \class QuadEdgeMeshPolygonCell
 * Class that connects the QE with itk
 *
 * \param TCellInterface Basic type for the itk*Cell. This usually comes
 *        from the MeshTraits.
 * \author Alexandre Gouaillard, Leonardo Florez-Valencia, Eric Boix
 *
 * This implementation was contributed as a paper to the Insight Journal
 * http://hdl.handle.net/1926/306
 *
 */
template< class TCellInterface >
class QuadEdgeMeshPolygonCell : public TCellInterface
{
public:
  /** Standard class typedefs. */
  // itkCellCommonTypedefs
  typedef QuadEdgeMeshPolygonCell   Self;
  typedef AutoPointer< const Self > ConstSelfAutoPointer;
  typedef AutoPointer< Self >       SelfAutoPointer;
  typedef Self*                     RawPointer;
  typedef const Self*               ConstRawPointer;
  
  // itkCellInheritedTypedefs
  typedef TCellInterface                                Superclass;
  typedef typename Superclass::PixelType                PixelType;
  typedef typename Superclass::CellType                 CellType;
  typedef typename Superclass::CellAutoPointer          CellAutoPointer;
  typedef typename Superclass::CellConstAutoPointer     CellConstAutoPointer;
  typedef typename Superclass::CellRawPointer           CellRawPointer;
  typedef typename Superclass::CellConstRawPointer      CellConstRawPointer;
  typedef typename Superclass::CellTraits               CellTraits;
  typedef typename Superclass::CoordRepType             CoordRepType;
  typedef typename Superclass::InterpolationWeightType  InterpolationWeightType;
  typedef typename Superclass::PointIdentifier          PointIdentifier;
  typedef typename Superclass::CellIdentifier           CellIdentifier;
  typedef typename Superclass::CellFeatureIdentifier    CellFeatureIdentifier;
  typedef typename Superclass::CellFeatureIdentifier    CellFeatureCount;
  typedef typename Superclass::PointType                PointType;
  typedef typename Superclass::PointsContainer          PointsContainer;
  typedef typename Superclass::UsingCellsContainer      UsingCellsContainer;
  typedef typename Superclass::CellGeometry             CellGeometry;
  typedef typename Superclass::ParametricCoordArrayType ParametricCoordArrayType;
  typedef typename Superclass::ShapeFunctionsArrayType  ShapeFunctionsArrayType;
  itkStaticConstMacro( PointDimension, unsigned int, Superclass::PointDimension );
  itkStaticConstMacro( CellDimension, unsigned int, 2 );
  
  /** Multivisitor type. */
  typedef typename CellType::MultiVisitor MultiVisitor;
  
  typedef QuadEdgeMeshLineCell< CellType >               EdgeCellType;
  typedef std::vector< EdgeCellType* >                   EdgeCellListType; 

  //** */
  typedef typename CellTraits::PointIdIterator              PointIdIterator;
  typedef typename CellTraits::PointIdConstIterator         PointIdConstIterator;
  typedef typename CellTraits::PointIdInternalIterator      PointIdInternalIterator;
  typedef typename CellTraits::PointIdInternalConstIterator PointIdInternalConstIterator;

  /** QE types. */
  typedef typename CellTraits::QuadEdgeType              QuadEdgeType;
  typedef typename QuadEdgeType::OriginRefType           VertexRefType;
  typedef typename QuadEdgeType::DualOriginRefType       FaceRefType;
  typedef typename QuadEdgeType::PrimalDataType          PrimalDataType;
  typedef typename QuadEdgeType::DualDataType            DualDataType;
  typedef typename QuadEdgeType::DualType                QEDual;
   
public:
  /** Standard part of every itk Object. */
  itkTypeMacro( QuadEdgeMeshPolygonCell, TCellInterface );
  
  /** Object memory management methods. */
  QuadEdgeMeshPolygonCell( PointIdentifier nPoints = 0 );
  QuadEdgeMeshPolygonCell( QuadEdgeType* e );
  virtual ~QuadEdgeMeshPolygonCell();
  
  /** Accessors for m_Ident. */
  void SetIdent( CellIdentifier cid ) { m_Ident = cid; }
  CellIdentifier GetIdent()          { return( m_Ident ); }
  
  /** Lnext ring entry accessors. */
  QuadEdgeType* GetEdgeRingEntry() const      { return( m_EdgeRingEntry ); }
  void SetEdgeRingEntry( QuadEdgeType* entry ) { m_EdgeRingEntry = entry; }
  
  /** Implement the standard CellInterface. */
  SelfAutoPointer New();
  
  /** TCellInterface abstract methods definition. */
  virtual void Accept( unsigned long cellId, MultiVisitor* mv );
  virtual CellGeometry GetType() const { return( Superclass::POLYGON_CELL );}
  
  /** itk topology related methods. */
  static int GetTopologyId()
    {
    return( Superclass::POLYGON_CELL );
    }
  virtual unsigned int GetDimension() const 
    { 
    return( Self::CellDimension );
    }
  virtual unsigned int GetNumberOfPoints() const;
  virtual CellFeatureCount GetNumberOfBoundaryFeatures( int dimension ) const;
  virtual bool GetBoundaryFeature( int dimension,
                                   CellFeatureIdentifier cellId,
                                   CellAutoPointer& cell );
  
  /** Useless methods. */
  virtual void MakeCopy( CellAutoPointer& cell ) const 
    { 
    const unsigned long numberOfPoints = this->GetNumberOfPoints();
    Self * newPolygonCell = new Self( numberOfPoints );
    cell.TakeOwnership( newPolygonCell );
    if ( numberOfPoints ) 
      {
      for( unsigned long i = 0; i < numberOfPoints; i++ )
        { 
        newPolygonCell->SetPointId( i, this->GetPointId( i ) );
        }
      }
    }

  
  /** ITK Cell API - Iterator-related methods.*/
  virtual void SetPointIds( PointIdConstIterator first );
  virtual void SetPointIds( PointIdConstIterator first,
                            PointIdConstIterator last );
  virtual void SetPointId( int localId, PointIdentifier pId );
  
  virtual PointIdentifier GetPointId( int localId ) const;

  virtual PointIdIterator PointIdsBegin()
    {
    // NOTE ALEX: should update the array on the fly to make it faster
    MakePointIds();
    if (m_PointIds.size() == 0)
      {
      return (static_cast<PointIdIterator>(0));
      }
    else
      {
      return &*(m_PointIds.begin());
      }
    }

  virtual PointIdIterator PointIdsEnd()
    {
    // NOTE ALEX: should update the array on the fly to make it faster
    // MakePointIds();
    if (m_PointIds.size() == 0)
      {
      return (static_cast<PointIdIterator>(0));
      }
    else
      {
      return &m_PointIds[m_PointIds.size()-1] + 1;
      }
    }

  virtual PointIdConstIterator PointIdsBegin() const
    {
    // NOTE ALEX: should update the array on the fly to make it faster
    MakePointIds();
    if (m_PointIds.size() == 0)
      {
      return (static_cast<PointIdIterator>(0));
      }
    else
      {
      return &*(m_PointIds.begin());
      }
    }

  virtual PointIdConstIterator PointIdsEnd() const
    {
    // NOTE ALEX: should update the array on the fly to make it faster
    // MakePointIds();
    if (m_PointIds.size() == 0)
      {
      return (static_cast<PointIdIterator>(0));
      }
    else
      {
      return &m_PointIds[m_PointIds.size()-1] + 1;
      }
    }

  /** QuadEdge internal flavor of cell API */
  virtual void InternalSetPointIds( PointIdInternalConstIterator first );
  virtual void InternalSetPointIds( PointIdInternalConstIterator first,
                            PointIdInternalConstIterator last );

  virtual PointIdInternalIterator InternalPointIdsBegin();
  virtual PointIdInternalIterator InternalPointIdsEnd();

  virtual PointIdInternalConstIterator InternalGetPointIds() const;
  virtual PointIdInternalConstIterator InternalPointIdsBegin() const;
  virtual PointIdInternalConstIterator InternalPointIdsEnd() const;

protected:
  typedef std::vector<PointIdentifier>  PointIDListType;
  mutable PointIDListType m_PointIds;

private:
  QuadEdgeMeshPolygonCell( const Self& ); // Not impl.
  void operator=( const Self& ); // Not impl.

  void MakePointIds() const
    {
    if( !this->GetNumberOfPoints( ) )
      {
      return;
      }

    // NOTE ALEX: very inefficient way of doing it ...
    // you wanna support old API, you pay for it.
    m_PointIds.clear();
    for( PointIdentifier i = 0; i < this->GetNumberOfPoints( ); i++ )
      {
      m_PointIds.push_back( GetPointId( i ) );
      }
    }

  /** In order to have constant time access at the itk level instead of
   * doing a search in the Mesh::Cell container.
   */
  CellIdentifier m_Ident;
  
  /**
   * Entry point into the edge ring.
   */
  QuadEdgeType * m_EdgeRingEntry;

  /**
   * List of EdgeCells created by the constructor for proper deletion
   */
  EdgeCellListType m_EdgeCellList;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkQuadEdgeMeshPolygonCell.txx"
#endif

#endif
