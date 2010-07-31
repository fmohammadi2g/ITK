/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageRegionConstIterator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageRegionConstIterator_h
#define __itkImageRegionConstIterator_h

#include "itkImageConstIterator.h"
#include "itkImageIterator.h"

namespace itk
{

/** \class ImageRegionConstIterator
 * \brief A multi-dimensional iterator templated over image type that walks a
 * region of pixels.
 *
 * The itk::ImageRegionConstIterator is optimized for iteration speed and is the
 * first choice for iterative, pixel-wise operations on an image.
 * ImageRegionIterator is the least specialized of the ITK image iterator
 * classes.  ImageRegionConstIterator is templated over the image type, and is
 * constrained to walk only within the specified region and along a line
 * parallel to one of the coordinate axes, "wrapping" to the next line as it
 * reaches the boundary of the image.  To walk the entire image, specify the
 * region to be \c image->GetRequestedRegion().
 *
 * ImageRegionConstIterator provides read-only access to image data.  It is the
 * base class for the read/write access ImageRegionIterator.
 *
 * ImageRegionConstIterator is a multi-dimensional iterator, requiring more
 * information be specified before the iterator can be used than conventional
 * iterators. Whereas the std::vector::iterator from the STL only needs to be
 * passed a pointer to establish the iterator, the multi-dimensional image
 * iterator needs a pointer, the size of the buffer, the size of the region,
 * the start index of the buffer, and the start index of the region. To gain
 * access to this information, ImageRegionConstIterator holds a reference to the
 * image over which it is traversing.
 *
 * ImageRegionConstIterator assumes a particular layout of the image data. The
 * is arranged in a 1D array as if it were [][][][slice][row][col] with
 * Index[0] = col, Index[1] = row, Index[2] = slice, etc.
 *
 * operator++ provides a simple syntax for walking around a region of
 * a multidimensional image. operator++ iterates across a row, constraining
 * the movement to within a region of image. When the iterator reaches
 * the boundary of the region along a row, the iterator automatically
 * wraps to the next row, starting at the first pixel in the row that is
 * part of the region. This allows for simple processing loops of the form:
 *
 * \code
 *
 *      it = it.Begin();
 *      for (; !it.IsAtEnd(); ++it)
 *         {
 *         *it += 100.0;
 *         }
 *
 * \endcode
 *
 *
 * \par MORE INFORMATION
 * For a complete description of the ITK Image Iterators and their API, please
 * see the Iterators chapter in the ITK Software Guide.  The ITK Software Guide
 * is available in print and as a free .pdf download from http://www.itk.org.
 *
 * \ingroup ImageIterators
 * \example Iterators/ImageRegionIterator.cxx
 *
 * \sa ImageConstIterator \sa ConditionalConstIterator
 * \sa ConstNeighborhoodIterator \sa ConstShapedNeighborhoodIterator
 * \sa ConstSliceIterator  \sa CorrespondenceDataStructureIterator 
 * \sa FloodFilledFunctionConditionalConstIterator 
 * \sa FloodFilledImageFunctionConditionalConstIterator 
 * \sa FloodFilledImageFunctionConditionalIterator 
 * \sa FloodFilledSpatialFunctionConditionalConstIterator 
 * \sa FloodFilledSpatialFunctionConditionalIterator 
 * \sa ImageConstIterator \sa ImageConstIteratorWithIndex 
 * \sa ImageIterator \sa ImageIteratorWithIndex
 * \sa ImageLinearConstIteratorWithIndex  \sa ImageLinearIteratorWithIndex 
 * \sa ImageRandomConstIteratorWithIndex  \sa ImageRandomIteratorWithIndex 
 * \sa ImageRegionConstIterator \sa ImageRegionConstIteratorWithIndex 
 * \sa ImageRegionExclusionConstIteratorWithIndex 
 * \sa ImageRegionExclusionIteratorWithIndex 
 * \sa ImageRegionIterator  \sa ImageRegionIteratorWithIndex 
 * \sa ImageRegionReverseConstIterator  \sa ImageRegionReverseIterator 
 * \sa ImageReverseConstIterator  \sa ImageReverseIterator 
 * \sa ImageSliceConstIteratorWithIndex  \sa ImageSliceIteratorWithIndex 
 * \sa NeighborhoodIterator \sa PathConstIterator  \sa PathIterator 
 * \sa ShapedNeighborhoodIterator  \sa SliceIterator 
 * \sa ImageConstIteratorWithIndex */
template<typename TImage>
class ITK_EXPORT ImageRegionConstIterator : public ImageConstIterator<TImage>
{
public:
  /** Standard class typedef. */
  typedef ImageRegionConstIterator    Self;
  typedef ImageConstIterator<TImage>  Superclass;
  
  /** Dimension of the image the iterator walks.  This constant is needed so
   * functions that are templated over image iterator type (as opposed to
   * being templated over pixel type and dimension) can have compile time
   * access to the dimension of the image that the iterator walks. */
  itkStaticConstMacro(ImageIteratorDimension, unsigned int,
                      Superclass::ImageIteratorDimension);

  /** Index typedef support. While this was already typdef'ed in the superclass
   * it needs to be redone here for this subclass to compile properly with gcc. */
  typedef typename Superclass::IndexType IndexType;

  /** Size typedef support. While this was already typdef'ed in the superclass
   * it needs to be redone here for this subclass to compile properly with gcc. */
  typedef typename Superclass::SizeType SizeType;

  /** Region typedef support. */
  typedef typename Superclass::RegionType   RegionType;

  /** Image typedef support. While this was already typdef'ed in the superclass
   * it needs to be redone here for this subclass to compile properly with gcc. */
  typedef typename Superclass::ImageType ImageType;

  /** PixelContainer typedef support. Used to refer to the container for
   * the pixel data. While this was already typdef'ed in the superclass
   * it needs to be redone here for this subclass to compile properly with gcc. */
  typedef typename Superclass::PixelContainer         PixelContainer;
  typedef typename Superclass::PixelContainerPointer  PixelContainerPointer;
  
  /** Internal Pixel Type */
  typedef typename Superclass::InternalPixelType   InternalPixelType;

  /** External Pixel Type */
  typedef typename Superclass::PixelType   PixelType;

  /**  Accessor type that convert data between internal and external
   *  representations. */
  typedef typename Superclass::AccessorType     AccessorType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageRegionConstIterator, ImageIterator);

  /** Default constructor. Needed since we provide a cast constructor. */
  ImageRegionConstIterator() : ImageConstIterator<TImage>()
    {
    m_SpanBeginOffset = 0;
    m_SpanEndOffset = 0;
    }
  
  /** Constructor establishes an iterator to walk a particular image and a
   * particular region of that image. */
  ImageRegionConstIterator(const ImageType *ptr,
                      const RegionType &region)
    : ImageConstIterator<TImage>(ptr, region)
    {
    m_SpanBeginOffset = this->m_BeginOffset;
    m_SpanEndOffset   = this->m_BeginOffset + static_cast<long>(this->m_Region.GetSize()[0]);
    }

  /** Constructor that can be used to cast from an ImageIterator to an
   * ImageRegionConstIterator. Many routines return an ImageIterator but for a
   * particular task, you may want an ImageRegionConstIterator.  Rather than
   * provide overloaded APIs that return different types of Iterators, itk
   * returns ImageIterators and uses constructors to cast from an
   * ImageIterator to a ImageRegionConstIterator. */
  ImageRegionConstIterator( const ImageIterator<TImage> &it)
    {
    this->ImageConstIterator<TImage>::operator=(it);
    IndexType ind = this->GetIndex();
    m_SpanEndOffset = this->m_Offset + static_cast<long>(this->m_Region.GetSize()[0]) 
      - (ind[0] - this->m_Region.GetIndex()[0]);
    m_SpanBeginOffset = m_SpanEndOffset
      - static_cast<long>(this->m_Region.GetSize()[0]);
    }

  /** Constructor that can be used to cast from an ImageConstIterator to an
   * ImageRegionConstIterator. Many routines return an ImageIterator but for a
   * particular task, you may want an ImageRegionConstIterator.  Rather than
   * provide overloaded APIs that return different types of Iterators, itk
   * returns ImageIterators and uses constructors to cast from an
   * ImageIterator to a ImageRegionConstIterator. */
  ImageRegionConstIterator( const ImageConstIterator<TImage> &it)
    {
    this->ImageConstIterator<TImage>::operator=(it);
    IndexType ind = this->GetIndex();
    m_SpanEndOffset = this->m_Offset + static_cast<long>(this->m_Region.GetSize()[0]) 
      - (ind[0] - this->m_Region.GetIndex()[0]);
    m_SpanBeginOffset = m_SpanEndOffset
      - static_cast<long>(this->m_Region.GetSize()[0]);
    }

 /** Move an iterator to the beginning of the region. "Begin" is
  * defined as the first pixel in the region. */
  void GoToBegin()
    {
    Superclass::GoToBegin();

    // reset the span offsets
    m_SpanBeginOffset = this->m_BeginOffset;
    m_SpanEndOffset   = this->m_BeginOffset + static_cast<long>(this->m_Region.GetSize()[0]);
    }
  
 /** Move an iterator to the end of the region. "End" is defined as
  * one pixel past the last pixel of the region. */
  void GoToEnd()
    {
    Superclass::GoToEnd();
    
    // reset the span offsets
    m_SpanEndOffset = this->m_EndOffset;
    m_SpanBeginOffset = m_SpanEndOffset - static_cast<long>(this->m_Region.GetSize()[0]);
    }

  /** Return an iterator for the beginning of the region. "Begin"
   * is defined as the first pixel in the region.
   * \deprecated Use GoToBegin() instead */
  Self Begin(void) const;

   /** Return an iterator for the end of the region. "End" is defined
   * as one pixel past the last pixel of the region. 
   * \deprecated Use GoToEnd() instead */
  Self End(void) const;


  /** Set the index. No bounds checking is performed. This is overridden
   * from the parent because we have an extra ivar.
   * \sa GetIndex */
  void SetIndex(const IndexType &ind)
    {
    Superclass::SetIndex(ind);
    m_SpanEndOffset = this->m_Offset + static_cast<long>(this->m_Region.GetSize()[0]) 
      - (ind[0] - this->m_Region.GetIndex()[0]);
    m_SpanBeginOffset = m_SpanEndOffset - static_cast<long>(this->m_Region.GetSize()[0]);
    }
  
  /** Increment (prefix) the fastest moving dimension of the iterator's index.
   * This operator will constrain the iterator within the region (i.e. the
   * iterator will automatically wrap from the end of the row of the region
   * to the beginning of the next row of the region) up until the iterator
   * tries to moves past the last pixel of the region.  Here, the iterator
   * will be set to be one pixel past the end of the region.
   * \sa operator++(int) */
  Self &
  operator++()
    {
    if (++this->m_Offset >= m_SpanEndOffset)
      {
      this->Increment();
      }
    return *this;
    }

  /** Decrement (prefix) the fastest moving dimension of the iterator's index.
   * This operator will constrain the iterator within the region (i.e. the
   * iterator will automatically wrap from the beginning of the row of the region
   * to the end of the next row of the region) up until the iterator
   * tries to moves past the first pixel of the region.  Here, the iterator
   * will be set to be one pixel past the beginning of the region.
   * \sa operator--(int) */
  Self & operator--()
    {
    if (--this->m_Offset < m_SpanBeginOffset)
      {
      this->Decrement();
      }
    return *this;
    }

 
protected:
  unsigned long m_SpanBeginOffset;  // one pixel before the beginning of the span (row)
  unsigned long m_SpanEndOffset;  // one pixel past the end of the span (row)
       
private:
  void Increment(); // advance in a direction other than the fastest moving
  void Decrement(); // go back in a direction other than the fastest moving
 
};

} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_ImageRegionConstIterator(_, EXPORT, x, y) namespace itk { \
  _(1(class EXPORT ImageRegionConstIterator< ITK_TEMPLATE_1 x >)) \
  namespace Templates { typedef ImageRegionConstIterator< ITK_TEMPLATE_1 x > ImageRegionConstIterator##y; } \
  }


#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkImageRegionConstIterator+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkImageRegionConstIterator.txx"
#endif

#endif
