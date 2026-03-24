
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   sdmaps.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Signed distance map and related calculations.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "sdmaps.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin sdmaps namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace sdmaps
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Base class for distance field generation from a boolean map.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool SDMap::Prepare( const IGetBool1D& Map )
{
	int w = Map.GetWidth();
	if( w < 1 )
	{
		return( false );
	}
	int n = Setup( w, 1, 1 );
	if( !n )
	{
		return( false );
	}
	int m = ( w * w );
	int32_t* p = reinterpret_cast< int32_t* >( m_pData );
	for( int x = 0; x < w; ++x )
	{	//	initialise
		*p = ( Map.GetBool( x ) ? m : -m );
		++p;
	}
	Set1D( w, 1, 1, 0, reinterpret_cast< int32_t* >( m_pData ) );
	Final( n, reinterpret_cast< int32_t* >( m_pData ) );
	return( true );
}

bool SDMap::Prepare( const IGetBool2D& Map )
{
	int w = Map.GetWidth();
	if( w < 1 )
	{
		return( false );
	}
	int h = Map.GetHeight();
	if( h < 1 )
	{
		h = 1;
	}
	int n = Setup( w, h, 1 );
	if( !n )
	{
		return( false );
	}
	int m = ( ( w * w ) + ( h * h ) );
	int32_t* p = reinterpret_cast< int32_t* >( m_pData );
	for( int y = 0; y < h; ++y )
	{	//	initialise
		for( int x = 0; x < w; ++x )
		{
			*p = ( Map.GetBool( x, y ) ? m : -m );
			++p;
		}
	}
	Set1D( w, h, 1, w, reinterpret_cast< int32_t* >( m_pData ) );
	if( h > 1 )
	{
		Set1D( h, w, w, 1, reinterpret_cast< int32_t* >( m_pData ) );
		Set2D( w, h, 1, w, reinterpret_cast< int32_t* >( m_pData ) );
	}
	Final( n, reinterpret_cast< int32_t* >( m_pData ) );
	return( true );
}

bool SDMap::Prepare( const IGetBool3D& Map )
{
	int w = Map.GetWidth();
	if( w < 1 )
	{
		return( false );
	}
	int h = Map.GetHeight();
	if( h < 1 )
	{
		h = 1;
	}
	int d = Map.GetDepth();
	if( d < 1 )
	{
		d = 1;
	}
	int n = Setup( w, h, d );
	if( !n )
	{
		return( false );
	}
	int m = ( ( w * w ) + ( h * h ) + ( d * d ) );
	int32_t* p = reinterpret_cast< int32_t* >( m_pData );
	for( int z = 0; z < d; ++z )
	{	//	initialise
		for( int y = 0; y < h; ++y )
		{
			for( int x = 0; x < w; ++x )
			{
				*p = ( Map.GetBool( x, y, z ) ? m : -m );
				++p;
			}
		}
	}
	Set1D( w, h, 1, w, reinterpret_cast< int32_t* >( m_pData ) );
	if( d > 1 )
	{
		Set1D( h, d, w, ( w * h ), reinterpret_cast< int32_t* >( m_pData ) );
		Set1D( d, w, ( w * h ), 1, reinterpret_cast< int32_t* >( m_pData ) );
		Set2D( w, h, 1, w, reinterpret_cast< int32_t* >( m_pData ) );
		Set2D( h, d, w, ( w * h ), reinterpret_cast< int32_t* >( m_pData ) );
		Set2D( d, w, ( w * h ), 1, reinterpret_cast< int32_t* >( m_pData ) );
		Set3D( w, h, d, 1, w, ( w * h ), reinterpret_cast< int32_t* >( m_pData ) );
	}
	else if( h > 1 )
	{
		Set1D( h, w, w, 1, reinterpret_cast< int32_t* >( m_pData ) );
		Set2D( w, h, 1, w, reinterpret_cast< int32_t* >( m_pData ) );
	}
	Final( n, reinterpret_cast< int32_t* >( m_pData ) );
	return( true );
}

bool SDMap::FillMap( ISetFloat1D& Map ) const
{
	if( m_pData == NULL )
	{
		return( false );
	}
	int w = Map.GetWidth();
	for( int x = 0; x < w; ++x )
	{
		float fu = ( ( x + 0.5f ) / w );
		Map.SetFloat( x, Fetch( fu ) );
	}
	return( true );
}

bool SDMap::FillMap( ISetFloat2D& Map ) const
{
	if( m_pData == NULL )
	{
		return( false );
	}
	int w = Map.GetWidth();
	int h = Map.GetHeight();
	for( int y = 0; y < h; ++y )
	{
		float fv = ( ( y + 0.5f ) / h );
		for( int x = 0; x < w; ++x )
		{
			float fu = ( ( x + 0.5f ) / w );
			Map.SetFloat( x, y, Fetch( fu, fv ) );
		}
	}
	return( true );
}

bool SDMap::FillMap( ISetFloat3D& Map ) const
{
	if( m_pData == NULL )
	{
		return( false );
	}
	int w = Map.GetWidth();
	int h = Map.GetHeight();
	int d = Map.GetDepth();
	for( int z = 0; z < d; ++z )
	{
		float fw = ( ( z + 0.5f ) / d );
		for( int y = 0; y < h; ++y )
		{
			float fv = ( ( y + 0.5f ) / h );
			for( int x = 0; x < w; ++x )
			{
				float fu = ( ( x + 0.5f ) / w );
				Map.SetFloat( x, y, z, Fetch( fu, fv, fw ) );
			}
		}
	}
	return( true );
}

void SDMap::Destroy( void )
{
	if( m_pData != NULL )
	{
		delete[] reinterpret_cast< int32_t* >( m_pData );
		m_pData = NULL;
	}
	m_Width = 0;
	m_Height = m_Depth = 1;
}

float SDMap::Fetch( const float u ) const
{
	int ua, ub;
	float ut = Index( u, m_Width, ua, ub );
	float l = reinterpret_cast< fp32* >( m_pData )[ ua ];
	float r = reinterpret_cast< fp32* >( m_pData )[ ub ];
	return( l + ( ( r - l ) * ut ) );
}

float SDMap::Fetch( const float u, const float v ) const
{
	int ua, ub;
	float ut = Index( u, m_Width, ua, ub );
	int va, vb;
	float vt = Index( v, m_Height, va, vb );
	va *= m_Width;
	vb *= m_Width;
	float tl = reinterpret_cast< fp32* >( m_pData )[ ua + va ];
	float tr = reinterpret_cast< fp32* >( m_pData )[ ub + va ];
	float bl = reinterpret_cast< fp32* >( m_pData )[ ua + vb ];
	float br = reinterpret_cast< fp32* >( m_pData )[ ub + vb ];
	return( tl +
		( ( tr - tl ) * ut ) +
		( ( bl - tl ) * vt ) +
		( ( br + tl - tr - bl ) * ut * vt ) );
}

float SDMap::Fetch( const float u, const float v, const float w ) const
{
	int ua, ub;
	float ut = Index( u, m_Width, ua, ub );
	int va, vb;
	float vt = Index( v, m_Height, va, vb );
	int wa, wb;
	float wt = Index( w, m_Depth, wa, wb );
	va *= m_Width;
	vb *= m_Width;
	wa *= ( m_Width * m_Height );
	wb *= ( m_Width * m_Height );
	float ntl = reinterpret_cast< fp32* >( m_pData )[ ua + va + wa ];
	float ntr = reinterpret_cast< fp32* >( m_pData )[ ub + va + wa ];
	float nbl = reinterpret_cast< fp32* >( m_pData )[ ua + vb + wa ];
	float nbr = reinterpret_cast< fp32* >( m_pData )[ ub + vb + wa ];
	float ftl = reinterpret_cast< fp32* >( m_pData )[ ua + va + wb ];
	float ftr = reinterpret_cast< fp32* >( m_pData )[ ub + va + wb ];
	float fbl = reinterpret_cast< fp32* >( m_pData )[ ua + vb + wb ];
	float fbr = reinterpret_cast< fp32* >( m_pData )[ ub + vb + wb ];
	return( ntl +
		( ( ( ( ntl - ntr + nbr - nbl ) * vt ) + ( ntr - ntl ) ) * ut ) +
		( ( ( ( ntl - nbl + fbl - ftl ) * wt ) + ( nbl - ntl ) ) * vt ) +
		( ( ( ( ntl - ftl + ftr - ntr ) * ut ) + ( ftl - ntl ) ) * wt ) +
		( ( fbr - fbl - ftr + ftl - nbr + nbl + ntr - ntl ) * ut * vt * wt ) );
}

float SDMap::Index( const float x, const int d, int& a, int& b ) const
{
	a = 0;
	b = ( d - 1 );
	float t = ( ( x * d ) - 0.5f );
	if( t <= 0 )
	{
		b = 0;
		t = 0;
	}
	else if( t >= b )
	{
		a = b;
		t = 1;
	}
	else
	{
		a = static_cast< int >( floorf( t ) );
		b = ( a + 1 );
		t -= a;
	}
	return( t );
}

int SDMap::Setup( const int w, const int h, const int d )
{
	if( ( m_pData == NULL ) || ( m_Width != w ) || ( m_Height != h ) || ( m_Depth != d ) )
	{
		if( m_pData != NULL )
		{
			delete[] reinterpret_cast< int32_t* >( m_pData );
		}
		m_pData = reinterpret_cast< void* >( new int32_t[ w * h * d ] );
		if( m_pData == NULL )
		{
			m_Width = 0;
			m_Height = m_Depth = 1;
			return( 0 );
		}
		m_Width = w;
		m_Height = h;
		m_Depth = d;
	}
	return( w * h * d );
}

void SDMap::Final( int n, int32_t* p )
{	//	convert integer distance squared to float distance and adjust for positive and negative spaces
	while( n )
	{
		int i = *p;
		float f = static_cast< float >( sqrt( fabs( static_cast< double >( i ) ) ) );
		*reinterpret_cast< fp32* >( p ) = ( ( i < 0 ) ? ( 0.5f - f ) : ( f - 0.5f ) );
		++p;
		--n;
	}
}

void SDMap::Set1D( const int nu, const int nv, const int du, const int dv, int32_t* const p )
{
	int uu = ( ( nu - 1 ) << 1 );
	int vv = ( ( nv - 1 ) << 1 );
	int sm = 0;			//	span sign mask
	int ss = du;		//	span sign step
	int32_t* pv = p;
	for( int vp = vv, vn = 0; vp >= 0; vp -= 2, vn += 2 )
	{
		int32_t* pu = pv;
		pv += dv;
		if( ( *pu ^ ss ) < 0 )
		{
			sm = ~sm;
			ss = -ss;
		}
		for( int up = uu, un = ( up - 2 ); up; up -= 2, un -= 4 )
		{	//	find sign transition
			pu += du;
			if( ( *pu ^ ss ) < 0 )
			{	//	found sign transition
				{	//	negative space
					int32_t* ps = ( pu + ( ss & sm ) );
					*ps = -1;
					int ds = ( up - ( un & sm ) );
					if( ds > 3 )
					{
						int ui = 3;
						int uj = -4;
						do
						{
							ps += ss;
							if( *ps < uj )
							{
								*ps = uj;
							}
							else if( *ps > 0 )
							{
								break;
							}
							ui += 2;
							uj -= ui;
						}
						while( ui < ds );
					}
				}
				sm = ~sm;
				ss = -ss;
				{	//	positive space
					int32_t* ps = ( pu + ( ss & sm ) );
					*ps = 1;
					int ds = ( up - ( un & sm ) );
					if( ds > 3 )
					{
						int ui = 3;
						int uj = 4;
						do
						{
							ps += ss;
							if( *ps > uj )
							{
								*ps = uj;
							}
							else if( *ps < 0 )
							{
								break;
							}
							ui += 2;
							uj += ui;
						}
						while( ui < ds );
					}
				}
			}
		}
	}
}

void SDMap::Set2D( const int nu, const int nv, const int du, const int dv, int32_t* const p )
{
	int uu = ( ( nu - 1 ) << 1 );
	int vv = ( ( nv - 1 ) << 1 );
	int sm = 0;			//	span sign mask
	int ss = du;		//	span sign step
	int32_t* pv = p;
	for( int vp = vv, vn = 0; vp >= 0; vp -= 2, vn += 2 )
	{
		int32_t* pu = pv;
		pv += dv;
		if( ( *pu ^ ss ) < 0 )
		{
			sm = ~sm;
			ss = -ss;
		}
		for( int up = uu, un = ( up - 2 ); up; up -= 2, un -= 4 )
		{	//	find sign transition
			pu += du;
			if( ( *pu ^ ss ) < 0 )
			{	//	found sign transition
				{	//	negative space
					int32_t* ps = ( pu + ( ss & sm ) );
					int ds = ( up - ( un & sm ) );
					int dp = vp;
					int dn = vn;
					int ui = 1;
					int uj = -2;
					do
					{
						if( dn )
						{
							int32_t* pd = ps;
							int vi = 1;
							int vj = uj;
							do
							{
								pd -= dv;
								if( *pd < vj )
								{
									*pd = vj;
								}
								else if( *pd > 0 )
								{
									dn = ( vi - 1 );
									break;
								}
								vi += 2;
								vj -= vi;
							}
							while( vi < dn );
						}
						if( dp )
						{
							int32_t* pd = ps;
							int vi = 1;
							int vj = uj;
							do
							{
								pd += dv;
								if( *pd < vj )
								{
									*pd = vj;
								}
								else if( *pd > 0 )
								{
									dp = ( vi - 1 );
									break;
								}
								vi += 2;
								vj -= vi;
							}
							while( vi < dp );
						}
						if( !( dn | dp ) )
						{
							break;
						}
						ps += ss;
						ui += 2;
						uj -= ui;
					}
					while( ui < ds );
				}
				sm = ~sm;
				ss = -ss;
				{	//	positive space
					int32_t* ps = ( pu + ( ss & sm ) );
					int ds = ( up - ( un & sm ) );
					int dp = vp;
					int dn = vn;
					int ui = 1;
					int uj = 2;
					do
					{
						if( dn )
						{
							int32_t* pd = ps;
							int vi = 1;
							int vj = uj;
							do
							{
								pd -= dv;
								if( *pd > vj )
								{
									*pd = vj;
								}
								else if( *pd < 0 )
								{
									dn = ( vi - 1 );
									break;
								}
								vi += 2;
								vj += vi;
							}
							while( vi < dn );
						}
						if( dp )
						{
							int32_t* pd = ps;
							int vi = 1;
							int vj = uj;
							do
							{
								pd += dv;
								if( *pd > vj )
								{
									*pd = vj;
								}
								else if( *pd < 0 )
								{
									dp = ( vi - 1 );
									break;
								}
								vi += 2;
								vj += vi;
							}
							while( vi < dp );
						}
						if( !( dn | dp ) )
						{
							break;
						}
						ps += ss;
						ui += 2;
						uj += ui;
					}
					while( ui < ds );
				}
			}
		}
	}
}

void SDMap::Set3D( const int nu, const int nv, const int nw, const int du, const int dv, const int dw, int32_t* const p )
{
	int uu = ( ( nu - 1 ) << 1 );
	int vv = ( ( nv - 1 ) << 1 );
	int ww = ( ( nw - 1 ) << 1 );
	int sm = 0;			//	span sign mask
	int ss = du;		//	span sign step
	int32_t* pw = p;
	for( int wp = ww, wn = 0; wp >= 0; wp -= 2, wn += 2 )
	{
		int32_t* pv = pw;
		pw += dw;
		for( int vp = vv, vn = 0; vp >= 0; vp -= 2, vn += 2 )
		{
			int32_t* pu = pv;
			pv += dv;
			if( ( *pu ^ ss ) < 0 )
			{
				sm = ~sm;
				ss = -ss;
			}
			for( int up = uu, un = ( up - 2 ); up; up -= 2, un -= 4 )
			{	//	find sign transition
				pu += du;
				if( ( *pu ^ ss ) < 0 )
				{	//	found sign transition
					{	//	negative space
						int32_t* ps = ( pu + ( ss & sm ) );
						int ds = ( up - ( un & sm ) );
						int dp = vp;
						int dn = vn;
						int np = wp;
						int nn = wn;
						int pp = wp;
						int pn = wn;
						int ui = 1;
						int uj = -3;
						do
						{
							if( dn )
							{
								int32_t* pd = ps;
								int vi = 1;
								int vj = uj;
								do
								{
									pd -= dv;
									if( nn )
									{
										int32_t* pe = pd;
										int wi = 1;
										int wj = vj;
										do
										{
											pe -= dw;
											if( *pe < wj )
											{
												*pe = wj;
											}
											else if( *pe > 0 )
											{
												nn = ( wi - 1 );
												break;
											}
											wi += 2;
											wj -= wi;
										}
										while( wi < nn );
									}
									if( np )
									{
										int32_t* pe = pd;
										int wi = 1;
										int wj = vj;
										do
										{
											pe += dw;
											if( *pe < wj )
											{
												*pe = wj;
											}
											else if( *pe > 0 )
											{
												np = ( wi - 1 );
												break;
											}
											wi += 2;
											wj -= wi;
										}
										while( wi < np );
									}
									if( !( nn | np ) )
									{
										dn = 0;
										break;
									}
									vi += 2;
									vj -= vi;
								}
								while( vi < dn );
							}
							if( dp )
							{
								int32_t* pd = ps;
								int vi = 1;
								int vj = uj;
								do
								{
									pd += dv;
									if( pn )
									{
										int32_t* pe = pd;
										int wi = 1;
										int wj = vj;
										do
										{
											pe -= dw;
											if( *pe < wj )
											{
												*pe = wj;
											}
											else if( *pe > 0 )
											{
												pn = ( wi - 1 );
												break;
											}
											wi += 2;
											wj -= wi;
										}
										while( wi < pn );
									}
									if( pp )
									{
										int32_t* pe = pd;
										int wi = 1;
										int wj = vj;
										do
										{
											pe += dw;
											if( *pe < wj )
											{
												*pe = wj;
											}
											else if( *pe > 0 )
											{
												pp = ( wi - 1 );
												break;
											}
											wi += 2;
											wj -= wi;
										}
										while( wi < pp );
									}
									if( !( pn | pp ) )
									{
										dp = 0;
										break;
									}
									vi += 2;
									vj -= vi;
								}
								while( vi < dp );
							}
							if( !( dn | dp ) )
							{
								break;
							}
							ps += ss;
							ui += 2;
							uj -= ui;
						}
						while( ui < ds );
					}
					sm = ~sm;
					ss = -ss;
					{	//	positive space
						int32_t* ps = ( pu + ( ss & sm ) );
						int ds = ( up - ( un & sm ) );
						int dp = vp;
						int dn = vn;
						int np = wp;
						int nn = wn;
						int pp = wp;
						int pn = wn;
						int ui = 1;
						int uj = 3;
						do
						{
							if( dn )
							{
								int32_t* pd = ps;
								int vi = 1;
								int vj = uj;
								do
								{
									pd -= dv;
									if( nn )
									{
										int32_t* pe = pd;
										int wi = 1;
										int wj = vj;
										do
										{
											pe -= dw;
											if( *pe > wj )
											{
												*pe = wj;
											}
											else if( *pe < 0 )
											{
												nn = ( wi - 1 );
												break;
											}
											wi += 2;
											wj += wi;
										}
										while( wi < nn );
									}
									if( np )
									{
										int32_t* pe = pd;
										int wi = 1;
										int wj = vj;
										do
										{
											pe += dw;
											if( *pe > wj )
											{
												*pe = wj;
											}
											else if( *pe < 0 )
											{
												np = ( wi - 1 );
												break;
											}
											wi += 2;
											wj += wi;
										}
										while( wi < np );
									}
									if( !( nn | np ) )
									{
										dn = 0;
										break;
									}
									vi += 2;
									vj += vi;
								}
								while( vi < dn );
							}
							if( dp )
							{
								int32_t* pd = ps;
								int vi = 1;
								int vj = uj;
								do
								{
									pd += dv;
									if( pn )
									{
										int32_t* pe = pd;
										int wi = 1;
										int wj = vj;
										do
										{
											pe -= dw;
											if( *pe > wj )
											{
												*pe = wj;
											}
											else if( *pe < 0 )
											{
												pn = ( wi - 1 );
												break;
											}
											wi += 2;
											wj += wi;
										}
										while( wi < pn );
									}
									if( pp )
									{
										int32_t* pe = pd;
										int wi = 1;
										int wj = vj;
										do
										{
											pe += dw;
											if( *pe > wj )
											{
												*pe = wj;
											}
											else if( *pe < 0 )
											{
												pp = ( wi - 1 );
												break;
											}
											wi += 2;
											wj += wi;
										}
										while( wi < pp );
									}
									if( !( pn | pp ) )
									{
										dp = 0;
										break;
									}
									vi += 2;
									vj += vi;
								}
								while( vi < dp );
							}
							if( !( dn | dp ) )
							{
								break;
							}
							ps += ss;
							ui += 2;
							uj += ui;
						}
						while( ui < ds );
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    int to bool bridging class for use by ConeMap and CylinderMap classes.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBridge : public IGetBool2D
{
public:
	inline					CBridge() : m_pInts( NULL ), m_Min( 0 ), m_Max( 0 ) {};
	inline					CBridge( const IGetInt2D& Ints, const int Min, const int Max ) { SetMap( Ints, Min, Max ); };
	inline					~CBridge() {};
	inline void				SetMap( const IGetInt2D& Ints, const int Min, const int Max ) { m_pInts = &Ints; m_Min = ( ( Max < Min ) ? Max : Min ); m_Max = ( ( Max > Min ) ? Max : Min ); };
	virtual int				GetWidth( void ) const;
	virtual int				GetHeight( void ) const;
	virtual bool			GetBool( const int x, const int y ) const;
protected:
	const IGetInt2D*		m_pInts;
	int						m_Min, m_Max;
};

int CBridge::GetWidth( void ) const
{
	return( m_pInts->GetWidth() );
}

int CBridge::GetHeight( void ) const
{
	return( m_pInts->GetHeight() );
}

bool CBridge::GetBool( const int x, const int y ) const
{
	int i = m_pInts->GetInt( x, y );
	return( ( ( i >= m_Min ) && ( i <= m_Max ) ) ? true : false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Conservative cone map generation from a 2D integer map.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ConeMap::Create( const IGetInt2D& Map, const int Min, const int Max )
{
	int w = Map.GetWidth();
	if( w < 1 )
	{
		return( false );
	}
	int h = Map.GetHeight();
	if( h < 1 )
	{
		h = 1;
	}
	int n = Setup( w, h, 1 );
	if( !n )
	{
		return( false );
	}
	fp32* p = reinterpret_cast< fp32* >( m_pData );
	for( int i = n; i; --i )
	{	//	initialise
		*p = 0;
		++p;
	}
	int Hi = ( ( Max > Min ) ? Max : Min );
	int Lo = ( ( Max < Min ) ? Max : Min );
	m_Map = &Map;
	m_Max = Hi;
	for( int Pass = Hi; Pass >= Lo; --Pass )
	{
		CBridge Remap;
		Remap.SetMap( Map, Pass, Max );
		SDMap2D Set;
		if( !Set.Prepare( Remap ) )
		{
			Destroy();
			m_Map = NULL;
			return( false );
		}
		m_Min = Pass;
		Set.FillMap( *this );
	}
	m_Map = NULL;
	return( true );
}

int ConeMap::GetWidth( void ) const
{
	return( m_Map->GetWidth() );
}

int ConeMap::GetHeight( void ) const
{
	return( m_Map->GetHeight() );
}

void ConeMap::SetFloat( const int x, const int y, const float f )
{
	if( f < 0.0f )
	{
		int i = m_Map->GetInt( x, y );
		if( i > m_Min )
		{
			fp32* p = &reinterpret_cast< fp32* >( m_pData )[ ( y * m_Width ) + x ];
			float r = -( ( f * ( m_Max - i ) ) / ( m_Min - i ) );
			if( ( *p > r ) || ( *p == 0 ) )
			{
				*p = r;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Cylinder map generation from a 2D integer map.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CylinderMap::Create( const IGetInt2D& Map, const int Min, const int Max )
{
	int w = Map.GetWidth();
	if( w < 1 )
	{
		return( false );
	}
	int h = Map.GetHeight();
	if( h < 1 )
	{
		h = 1;
	}
	int n = Setup( w, h, 1 );
	if( !n )
	{
		return( false );
	}
	fp32* p = reinterpret_cast< fp32* >( m_pData );
	for( int i = n; i; --i )
	{	//	initialise
		*p = 0;
		++p;
	}
	int Hi = ( ( Max > Min ) ? Max : Min );
	int Lo = ( ( Max < Min ) ? Max : Min );
	m_Map = &Map;
	m_Max = Hi;
	for( int Pass = Hi; Pass >= Lo; --Pass )
	{
		CBridge Remap;
		Remap.SetMap( Map, Pass, Max );
		SDMap2D Set;
		if( !Set.Prepare( Remap ) )
		{
			Destroy();
			m_Map = NULL;
			return( false );
		}
		m_Min = Pass;
		Set.FillMap( *this );
	}
	m_Map = NULL;
	return( true );
}

int CylinderMap::GetWidth( void ) const
{
	return( m_Map->GetWidth() );
}

int CylinderMap::GetHeight( void ) const
{
	return( m_Map->GetHeight() );
}

void CylinderMap::SetFloat( const int x, const int y, const float f )
{
	if( f < 0.0f )
	{
		int i = m_Map->GetInt( x, y );
		if( i == m_Min )
		{
			reinterpret_cast< fp32* >( m_pData )[ ( y * m_Width ) + x ] = -f;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Standard bitmap bool input classes.
////
////    Input is from an 8-bit bitmap channel.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBmpGetBool1D::SetMap( uint8_t* const pData, const int Width, const int Stride )
{
	m_pData = pData;
	m_Width = Width;
	m_Stride = ( Stride ? Stride : 1 );
}

int CBmpGetBool1D::GetWidth( void ) const
{
	return( m_Width );
}

bool CBmpGetBool1D::GetBool( const int x ) const
{
	return( ( m_pData[ x * m_Stride ] < 128 ) ? false : true );
}

void CBmpGetBool2D::SetMap( uint8_t* const pData, const int Width, const int Height, const int Stride, const int Pitch )
{
	m_pData = pData;
	m_Width = Width;
	m_Height = ( ( Height < 1 ) ? 1 : Height );
	m_Stride = ( Stride ? Stride : 1 );
	m_Pitch = ( Pitch ? Pitch : ( m_Width * m_Stride ) );
}

int CBmpGetBool2D::GetWidth( void ) const
{
	return( m_Width );
}

int CBmpGetBool2D::GetHeight( void ) const
{
	return( m_Height );
}

bool CBmpGetBool2D::GetBool( const int x, const int y ) const
{
	return( ( m_pData[ ( y * m_Pitch ) + ( x * m_Stride ) ] < 128 ) ? false : true );
}

void CBmpGetBool3D::SetMap( uint8_t* const pData, const int Width, const int Height, const int Depth, const int Stride, const int Pitch, const int Layer )
{
	m_pData = pData;
	m_Width = Width;
	m_Height = ( ( Height < 1 ) ? 1 : Height );
	m_Depth = ( ( Depth < 1 ) ? 1 : Depth );
	m_Stride = ( Stride ? Stride : 1 );
	m_Pitch = ( Pitch ? Pitch : ( m_Width * m_Stride ) );
	m_Layer = ( Layer ? Layer : ( m_Height * m_Pitch ) );
}

int CBmpGetBool3D::GetWidth( void ) const
{
	return( m_Width );
}

int CBmpGetBool3D::GetHeight( void ) const
{
	return( m_Height );
}

int CBmpGetBool3D::GetDepth( void ) const
{
	return( m_Depth );
}

bool CBmpGetBool3D::GetBool( const int x, const int y, const int z ) const
{
	return( ( m_pData[ ( z * m_Layer ) + ( y * m_Pitch ) + ( x * m_Stride ) ] < 128 ) ? false : true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Standard bitmap int input classes.
////
////    Input is from an 8-bit bitmap channel.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBmpGetInt1D::SetMap( uint8_t* const pData, const int Width, const int Stride )
{
	m_pData = pData;
	m_Width = Width;
	m_Stride = ( Stride ? Stride : 1 );
}

int CBmpGetInt1D::GetWidth( void ) const
{
	return( m_Width );
}

int CBmpGetInt1D::GetInt( const int x ) const
{
	return( m_pData[ x * m_Stride ] );
}

void CBmpGetInt2D::SetMap( uint8_t* const pData, const int Width, const int Height, const int Stride, const int Pitch )
{
	m_pData = pData;
	m_Width = Width;
	m_Height = ( ( Height < 1 ) ? 1 : Height );
	m_Stride = ( Stride ? Stride : 1 );
	m_Pitch = ( Pitch ? Pitch : ( m_Width * m_Stride ) );
}

int CBmpGetInt2D::GetWidth( void ) const
{
	return( m_Width );
}

int CBmpGetInt2D::GetHeight( void ) const
{
	return( m_Height );
}

int CBmpGetInt2D::GetInt( const int x, const int y ) const
{
	return( m_pData[ ( y * m_Pitch ) + ( x * m_Stride ) ] );
}

void CBmpGetInt3D::SetMap( uint8_t* const pData, const int Width, const int Height, const int Depth, const int Stride, const int Pitch, const int Layer )
{
	m_pData = pData;
	m_Width = Width;
	m_Height = ( ( Height < 1 ) ? 1 : Height );
	m_Depth = ( ( Depth < 1 ) ? 1 : Depth );
	m_Stride = ( Stride ? Stride : 1 );
	m_Pitch = ( Pitch ? Pitch : ( m_Width * m_Stride ) );
	m_Layer = ( Layer ? Layer : ( m_Height * m_Pitch ) );
}

int CBmpGetInt3D::GetWidth( void ) const
{
	return( m_Width );
}

int CBmpGetInt3D::GetHeight( void ) const
{
	return( m_Height );
}

int CBmpGetInt3D::GetDepth( void ) const
{
	return( m_Depth );
}

int CBmpGetInt3D::GetInt( const int x, const int y, const int z ) const
{
	return( m_pData[ ( z * m_Layer ) + ( y * m_Pitch ) + ( x * m_Stride ) ] );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Standard bitmap output classes.
////
////    Output is to an 8-bit bitmap channel.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBmpSetFloat1D::SetMap( uint8_t* const pData, const int Width, const int Stride )
{
	m_pData = pData;
	m_Width = Width;
	m_Stride = ( Stride ? Stride : 1 );
}

void CBmpSetFloat1D::SetRange( const float Map0x00, const float Map0xff )
{
	m_Scale = ( 256.0f / ( Map0xff - Map0x00 ) );
	m_Offset = -( Map0x00 * m_Scale );
}

int CBmpSetFloat1D::GetWidth( void ) const
{
	return( m_Width );
}

void CBmpSetFloat1D::SetFloat( const int x, const float f )
{
	float v = ( ( f * m_Scale ) + m_Offset );
	uint8_t b = 0;
	if( v > 0 )
	{
		b = 255;
		if( v < 255 )
		{
			b = static_cast< uint8_t >( floorf( v ) );
		}
	}
	m_pData[ x * m_Stride ] = b;
}

void CBmpSetFloat2D::SetMap( uint8_t* const pData, const int Width, const int Height, const int Stride, const int Pitch )
{
	m_pData = pData;
	m_Width = Width;
	m_Height = ( ( Height < 1 ) ? 1 : Height );
	m_Stride = ( Stride ? Stride : 1 );
	m_Pitch = ( Pitch ? Pitch : ( m_Width * m_Stride ) );
}

void CBmpSetFloat2D::SetRange( const float Map0x00, const float Map0xff )
{
	m_Scale = ( 256.0f / ( Map0xff - Map0x00 ) );
	m_Offset = -( Map0x00 * m_Scale );
}

int CBmpSetFloat2D::GetWidth( void ) const
{
	return( m_Width );
}

int CBmpSetFloat2D::GetHeight( void ) const
{
	return( m_Height );
}

void CBmpSetFloat2D::SetFloat( const int x, const int y, const float f )
{
	float v = ( ( f * m_Scale ) + m_Offset );
	uint8_t b = 0;
	if( v > 0 )
	{
		b = 255;
		if( v < 255 )
		{
			b = static_cast< uint8_t >( floorf( v ) );
		}
	}
	m_pData[ ( y * m_Pitch ) + ( x * m_Stride ) ] = b;
}

void CBmpSetFloat3D::SetMap( uint8_t* const pData, const int Width, const int Height, const int Depth, const int Stride, const int Pitch, const int Layer )
{
	m_pData = pData;
	m_Width = Width;
	m_Height = ( ( Height < 1 ) ? 1 : Height );
	m_Depth = ( ( Depth < 1 ) ? 1 : Depth );
	m_Stride = ( Stride ? Stride : 1 );
	m_Pitch = ( Pitch ? Pitch : ( m_Width * m_Stride ) );
	m_Layer = ( Layer ? Layer : ( m_Height * m_Pitch ) );
}

void CBmpSetFloat3D::SetRange( const float Map0x00, const float Map0xff )
{
	m_Scale = ( 256.0f / ( Map0xff - Map0x00 ) );
	m_Offset = -( Map0x00 * m_Scale );
}

int CBmpSetFloat3D::GetWidth( void ) const
{
	return( m_Width );
}

int CBmpSetFloat3D::GetHeight( void ) const
{
	return( m_Height );
}

int CBmpSetFloat3D::GetDepth( void ) const
{
	return( m_Depth );
}

void CBmpSetFloat3D::SetFloat( const int x, const int y, const int z, const float f )
{
	float v = ( ( f * m_Scale ) + m_Offset );
	uint8_t b = 0;
	if( v > 0 )
	{
		b = 255;
		if( v < 255 )
		{
			b = static_cast< uint8_t >( floorf( v ) );
		}
	}
	m_pData[ ( z * m_Layer ) + ( y * m_Pitch ) + ( x * m_Stride ) ] = b;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end sdmaps namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace sdmaps

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
