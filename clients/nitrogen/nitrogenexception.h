#ifndef nitrogenexception_h
#define nitrogenexception_h

//////////////////////////////////////////////////////////////////////////////
// nitrogenexception.h
// -------------------
// 
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.                 
//////////////////////////////////////////////////////////////////////////////

#include <QRegExp>
#include "nitrogenconfiguration.h"


namespace NitrogenConfig
{
  
  //! needed for exceptions
  static const QString TYPE = "Type";
  static const QString PATTERN = "Pattern";
  static const QString ENABLED = "Enabled";
  static const QString MASK = "Mask";
    
}

namespace Nitrogen
{
  
  //! nitrogen exception
  class NitrogenException: public NitrogenConfiguration
  {
    
    public:
    
    //! exception type
    enum Type
    {
      WindowTitle,
      WindowClassName
    };
    
    //! mask
    enum AttributesMask
    {
      None = 0,
      TitleAlignment = 1<<0,
      ShowStripes = 1<<1,
      DrawSeparator = 1<<2,
      TitleOutline = 1<<3,
      FrameBorder = 1<<4,
      BlendColor = 1<<5,
      SizeGripMode = 1<<6,
      All = TitleAlignment|ShowStripes|DrawSeparator|TitleOutline|FrameBorder|BlendColor|SizeGripMode
    };
    
    //! constructor
    NitrogenException( NitrogenConfiguration configuration = NitrogenConfiguration() ):
      NitrogenConfiguration( configuration ),
      enabled_( true ),
      type_( WindowClassName ),
      mask_( None )
    {}

    //! constructor
    NitrogenException( KConfigGroup );
    
    //! destructor
    virtual ~NitrogenException( void )
    {}
    
    //! equal to operator
    bool operator == (const NitrogenException& exception ) const
    { 
      return
        enabled() == exception.enabled() &&
        type() == exception.type() &&
        regExp().pattern() == exception.regExp().pattern() &&
        mask() == exception.mask() &&
        NitrogenConfiguration::operator == ( exception );
    }

    //! less than operator
    bool operator < (const NitrogenException& exception ) const
    { 
      if( enabled() != exception.enabled() ) return enabled() < exception.enabled();
      else if( mask() != exception.mask() ) return mask() < exception.mask();
      else if( type() != exception.type() ) return type() < exception.type();
      else return regExp().pattern() < exception.regExp().pattern(); 
    }
     
    //! write to kconfig group
    virtual void write( KConfigGroup& ) const;
    
    //!@name enability
    //@{
    
    bool enabled( void ) const
    { return enabled_; }
    
    void setEnabled( bool enabled ) 
    { enabled_ = enabled; }
    
    //@}
    
    //!@name exception type
    //@{
    
    static QString typeName( Type );
    static Type type( const QString& name );

    virtual QString typeName( void ) const
    { return typeName( type() ); }
    
    virtual Type type( void ) const
    { return type_; }
    
    virtual void setType( Type value )
    { type_ = value; }

    //@}
    
    //!@name regular expression
    //@{

    virtual QRegExp regExp( void ) const
    { return reg_exp_; }

    virtual QRegExp& regExp( void ) 
    { return reg_exp_; }

    //@}
    
    
    //! mask
    //!@{
    
    unsigned int mask( void ) const
    { return mask_; }
    
    void setMask( unsigned int mask )
    { mask_ = mask; }
    
    //@}
        
    private:
    
    //! enability
    bool enabled_;
    
    //! exception type
    Type type_;
    
    //! regular expression to match window caption
    QRegExp reg_exp_;

    //! attributes mask
    unsigned int mask_;
    
  };
  
  
  
}

#endif
