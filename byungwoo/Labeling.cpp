#include "StdAfx.h"
#include "Labeling.h"

Labeling::Labeling(void)
{
	List = NULL;
	CheckField = NULL;
	Result = NULL;
}

Labeling::~Labeling(void)
{
	DestroyList();
	if( CheckField != NULL )
	{
	delete [] CheckField;
	CheckField = NULL;
	}
	if( Result != NULL )
	{
		delete [] Result;
		Result = NULL;
	}
}
void Labeling::MakeResultArray()
{
	if( List == NULL )
	{
		ObjectCount = 0;
		return;
	}

	ObjectCount = List->NodeCount;
	Result = new ResultArray[ List->NodeCount ];

	ListNode* NodeTemp = List->Top;
	int i = 0;
	while( NodeTemp != NULL )
	{
		Result[i].x = NodeTemp->Data->Start.x;
		Result[i].y = NodeTemp->Data->Start.y;
		Result[i].width = NodeTemp->Data->Width;
		Result[i].height = NodeTemp->Data->Height;
		i++;
		NodeTemp = NodeTemp->next;
	}


	
}

void Labeling::_UnDoLabeling( int x, int y, unsigned char* frame )
{
	Point PointTemp;
	PointTemp.x = x;
	PointTemp.y = y;
	CheckField[ y*Width + x ].PreLocation = PointTemp;

	while(1)
	{
		frame[ PointTemp.y*Width + PointTemp.x ] = BackgroundValue;
		// ↑ 방향
		if( PointTemp.y > 0 )
		{
			if( frame[ ( PointTemp.y - 1 )*Width + PointTemp.x ] == ObjectValue && CheckField[ ( PointTemp.y - 1 )*Width + PointTemp.x ].IsVisit == 1 )
			{
				CheckField[ ( PointTemp.y - 1 )*Width + PointTemp.x ].PreLocation = PointTemp;
				PointTemp.y -= 1;
				continue;
			}
		}
		// 45 방향
		if( PointTemp.y > 0 && PointTemp.x < Width -1 && ExploreMode == 1)
		{
			if( frame[ ( PointTemp.y - 1 ) * Width + PointTemp.x + 1 ] == ObjectValue && CheckField[  ( PointTemp.y - 1 ) * Width + PointTemp.x + 1 ].IsVisit == 1 )				
			{
				CheckField[ ( PointTemp.y - 1 ) * Width + PointTemp.x + 1 ].PreLocation = PointTemp;
				PointTemp.x += 1;
				PointTemp.y -= 1;
				continue;				
			}
		}
		// → 방향
		if( PointTemp.x < Width - 1 )
		{
			if( frame[ PointTemp.y * Width + PointTemp.x + 1 ] == ObjectValue && CheckField[  PointTemp.y * Width + PointTemp.x + 1 ].IsVisit == 1 )
			{
				CheckField[ PointTemp.y * Width + PointTemp.x + 1 ].PreLocation = PointTemp;
				PointTemp.x += 1;
				continue;
			}
		}
		// 90 + 45 방향
		if( PointTemp.x < Width - 1 && PointTemp.y < Height - 1 && ExploreMode == 1 )
		{
			if( frame[ ( PointTemp.y + 1 ) * Width + PointTemp.x + 1 ] && CheckField[ ( PointTemp.y + 1 ) * Width + PointTemp.x + 1 ].IsVisit == 1 )
			{
				CheckField[ ( PointTemp.y + 1 ) * Width + PointTemp.x + 1 ].PreLocation = PointTemp;
				PointTemp.x += 1;
				PointTemp.y += 1;
				continue;
			}
		}

		// ↓방향
		if( PointTemp.y < Height - 1 )
		{
			if( frame[ ( PointTemp.y + 1 )*Width + PointTemp.x ] == ObjectValue && CheckField[ ( PointTemp.y + 1 )*Width + PointTemp.x ].IsVisit == 1 )
			{
				CheckField[ ( PointTemp.y + 1 )*Width + PointTemp.x ].PreLocation = PointTemp;
				PointTemp.y += 1;
				continue;
			}
		}
		// 180 + 45 방향
		if( PointTemp.x > 0 && PointTemp.y < Height - 1 && ExploreMode == 1 )
		{
			if( frame[ ( PointTemp.y + 1 ) * Width + PointTemp.x - 1 ] && CheckField[ ( PointTemp.y + 1 ) * Width + PointTemp.x - 1 ].IsVisit == 1 )
			{
				CheckField[ ( PointTemp.y + 1 ) * Width + PointTemp.x - 1 ].PreLocation = PointTemp;
				PointTemp.x -= 1;
				PointTemp.y += 1;				
				continue;
			}
		}
		// ← 방향
		if( PointTemp.x > 0 )
		{
			if( frame[ PointTemp.y * Width + PointTemp.x - 1 ] == ObjectValue && CheckField[  PointTemp.y * Width + PointTemp.x - 1 ].IsVisit == 1 )
			{
				CheckField[ PointTemp.y * Width + PointTemp.x - 1 ].PreLocation = PointTemp;
				PointTemp.x -= 1;
				continue;
			}
		}
		// -45 방향
		if( PointTemp.x > 0 && PointTemp.y > 0 && ExploreMode == 1 )
		{
			if( frame[ ( PointTemp.y - 1 ) * Width + PointTemp.x - 1 ] && CheckField[ ( PointTemp.y - 1 ) * Width + PointTemp.x - 1 ].IsVisit == 1 )
			{
				CheckField[ ( PointTemp.y - 1 ) * Width + PointTemp.x - 1 ].PreLocation = PointTemp;			
				PointTemp.x -= 1;
				PointTemp.y -= 1;				
				continue;
			}
		}

		


		if( CheckField[ PointTemp.y * Width + PointTemp.x ].PreLocation.x == PointTemp.x && CheckField[ PointTemp.y * Width + PointTemp.x ].PreLocation.y == PointTemp.y )
			break;
		PointTemp = CheckField[ PointTemp.y * Width + PointTemp.x ].PreLocation;
	}
}


void Labeling::DoLabeling( unsigned char* frame )
{
	for( int y = 0 ; y < Height ; y++ )
	{
		for( int x = 0 ; x < Width ; x++ )
		{
			if( frame[ y*Width + x ] == ObjectValue && CheckField[ y*Width + x ].IsVisit == 0 )
			{
				int check = _DoLabeling( x, y, frame );
				if( check == 0 )
				{
					if( FilterMode == 0 )
						_UnDoLabeling( x, y, frame );
					DeleteLastNode();
				}
			}

		}
	}
	
	MakeResultArray();
	
}
int Labeling::_DoLabeling( int x, int y, unsigned char* frame )
{
	Object* ObjectTemp = MakeNewObject( x, y );
	Point PointTemp;
	PointTemp.x = x;
	PointTemp.y = y;

	CheckField[ y*Width + x ].IsVisit = 1;
	CheckField[ y*Width + x ].PreLocation = PointTemp;

	while(1)
	{
		// ↑ 방향
		if( PointTemp.y > 0 )
		{
			if( frame[ ( PointTemp.y - 1 )*Width + PointTemp.x ] == ObjectValue && CheckField[ ( PointTemp.y - 1 )*Width + PointTemp.x ].IsVisit == 0 )
			{
				CheckField[ ( PointTemp.y - 1 )*Width + PointTemp.x ].PreLocation = PointTemp;
				CheckField[ ( PointTemp.y - 1 )*Width + PointTemp.x ].IsVisit = 1;
				PointTemp.y -= 1;
				ObjectTemp->PixelCount += 1;
				if( ObjectTemp->Start.y > PointTemp.y )
				{
					ObjectTemp->Height += ObjectTemp->Start.y - PointTemp.y;
					ObjectTemp->Start.y = PointTemp.y;
				}
				continue;
			}
		}

		
		// ↗ 방향
		if( PointTemp.y > 0 && PointTemp.x < Width -1 && ExploreMode == 1)
		{
			if( frame[ ( PointTemp.y - 1 ) * Width + PointTemp.x + 1 ] == ObjectValue && CheckField[  ( PointTemp.y - 1 ) * Width + PointTemp.x + 1 ].IsVisit == 0 )				
			{
				CheckField[ ( PointTemp.y - 1 ) * Width + PointTemp.x + 1 ].PreLocation = PointTemp;
				CheckField[ ( PointTemp.y - 1 ) * Width + PointTemp.x + 1 ].IsVisit = 1;
				PointTemp.x += 1;
				PointTemp.y -= 1;
				ObjectTemp->PixelCount += 1;
				if( ObjectTemp->Start.x + ObjectTemp->Width - 1 < PointTemp.x )
				{
				ObjectTemp->Width = PointTemp.x - ObjectTemp->Start.x + 1;
				}
				if( ObjectTemp->Start.y > PointTemp.y )
				{
				ObjectTemp->Height += ObjectTemp->Start.y - PointTemp.y;
				ObjectTemp->Start.y = PointTemp.y;
				}
				continue;				
			}
		}
		
		// → 방향
		if( PointTemp.x < Width - 1 )
		{
			if( frame[ PointTemp.y * Width + PointTemp.x + 1 ] == ObjectValue && CheckField[  PointTemp.y * Width + PointTemp.x + 1 ].IsVisit == 0 )
			{
				CheckField[ PointTemp.y * Width + PointTemp.x + 1 ].PreLocation = PointTemp;
				CheckField[ PointTemp.y * Width + PointTemp.x + 1 ].IsVisit = 1;
				PointTemp.x += 1;
				ObjectTemp->PixelCount += 1;
				if( ObjectTemp->Start.x + ObjectTemp->Width - 1 < PointTemp.x )
				{
					ObjectTemp->Width = PointTemp.x - ObjectTemp->Start.x + 1;
				}
				continue;
			}
		}
		// ↘ 방향
		if( PointTemp.x < Width - 1 && PointTemp.y < Height - 1 && ExploreMode == 1 )
		{
			if( frame[ ( PointTemp.y + 1 ) * Width + PointTemp.x + 1 ] && CheckField[ ( PointTemp.y + 1 ) * Width + PointTemp.x + 1 ].IsVisit == 0 )
			{
				CheckField[ ( PointTemp.y + 1 ) * Width + PointTemp.x + 1 ].PreLocation = PointTemp;
				CheckField[ ( PointTemp.y + 1 ) * Width + PointTemp.x + 1 ].IsVisit = 1;
				PointTemp.x += 1;
				PointTemp.y += 1;
				ObjectTemp->PixelCount += 1;
				if( ObjectTemp->Start.x + ObjectTemp->Width - 1 < PointTemp.x )
				{
					ObjectTemp->Width = PointTemp.x - ObjectTemp->Start.x + 1;
				}
				if( ObjectTemp->Start.y + ObjectTemp->Height - 1 < PointTemp.y )
				{
					ObjectTemp->Height = PointTemp.y - ObjectTemp->Start.y + 1;
				}
				continue;
			}
		}


		// ↓방향
		if( PointTemp.y < Height - 1 )
		{
			if( frame[ ( PointTemp.y + 1 )*Width + PointTemp.x ] == ObjectValue && CheckField[ ( PointTemp.y + 1 )*Width + PointTemp.x ].IsVisit == 0 )
			{
				CheckField[ ( PointTemp.y + 1 )*Width + PointTemp.x ].PreLocation = PointTemp;
				CheckField[ ( PointTemp.y + 1 )*Width + PointTemp.x ].IsVisit = 1;
				PointTemp.y += 1;
				ObjectTemp->PixelCount += 1;
				if( ObjectTemp->Start.y + ObjectTemp->Height - 1 < PointTemp.y )
				{
					ObjectTemp->Height = PointTemp.y - ObjectTemp->Start.y + 1;
				}
				continue;
			}
		}
		// ↙ 방향
		if( PointTemp.x > 0 && PointTemp.y < Height - 1 && ExploreMode == 1 )
		{
			if( frame[ ( PointTemp.y + 1 ) * Width + PointTemp.x - 1 ] && CheckField[ ( PointTemp.y + 1 ) * Width + PointTemp.x - 1 ].IsVisit == 0 )
			{
				CheckField[ ( PointTemp.y + 1 ) * Width + PointTemp.x - 1 ].PreLocation = PointTemp;
				CheckField[ ( PointTemp.y + 1 ) * Width + PointTemp.x - 1 ].IsVisit = 1;
				PointTemp.x -= 1;
				PointTemp.y += 1;
				ObjectTemp->PixelCount += 1;
				if( ObjectTemp->Start.x > PointTemp.x )
				{
					ObjectTemp->Width += ObjectTemp->Start.x - PointTemp.x;
					ObjectTemp->Start.x = PointTemp.x;					
				}
				if( ObjectTemp->Start.y + ObjectTemp->Height - 1 < PointTemp.y )
				{
					ObjectTemp->Height = PointTemp.y - ObjectTemp->Start.y + 1;
				}
				
				continue;
			}
		}
		
		// ← 방향
		if( PointTemp.x > 0 )
		{
			if( frame[ PointTemp.y * Width + PointTemp.x - 1 ] == ObjectValue && CheckField[  PointTemp.y * Width + PointTemp.x - 1 ].IsVisit == 0 )
			{
				CheckField[ PointTemp.y * Width + PointTemp.x - 1 ].PreLocation = PointTemp;
				CheckField[ PointTemp.y * Width + PointTemp.x - 1 ].IsVisit = 1;
				PointTemp.x -= 1;
				ObjectTemp->PixelCount += 1;
				if( ObjectTemp->Start.x > PointTemp.x )
				{
					ObjectTemp->Width += ObjectTemp->Start.x - PointTemp.x;
					ObjectTemp->Start.x = PointTemp.x;					
				}
				continue;
			}
		}
		// ↖ 방향
		if( PointTemp.x > 0 && PointTemp.y > 0 && ExploreMode == 1 )
		{
			if( frame[ ( PointTemp.y - 1 ) * Width + PointTemp.x - 1 ] && CheckField[ ( PointTemp.y - 1 ) * Width + PointTemp.x - 1 ].IsVisit == 0 )
			{
				CheckField[ ( PointTemp.y - 1 ) * Width + PointTemp.x - 1 ].PreLocation = PointTemp;
				CheckField[ ( PointTemp.y - 1 ) * Width + PointTemp.x - 1 ].IsVisit = 1;
				PointTemp.x -= 1;
				PointTemp.y -= 1;
				ObjectTemp->PixelCount += 1;

				if( ObjectTemp->Start.x > PointTemp.x )
				{
					ObjectTemp->Width += ObjectTemp->Start.x - PointTemp.x;
					ObjectTemp->Start.x = PointTemp.x;					
				}
				if( ObjectTemp->Start.y > PointTemp.y )
				{
					ObjectTemp->Height += ObjectTemp->Start.y - PointTemp.y;
					ObjectTemp->Start.y = PointTemp.y;
				}
				
				
				continue;
			}
		}

		if( CheckField[ PointTemp.y * Width + PointTemp.x ].PreLocation.x == PointTemp.x && CheckField[ PointTemp.y * Width + PointTemp.x ].PreLocation.y == PointTemp.y )
			break;
		

		PointTemp = CheckField[ PointTemp.y * Width + PointTemp.x ].PreLocation;

		
	
	}
	

	PushList( ObjectTemp );
	// Object 판별 기준이 pixel count 일때
	if( ObjectMode == 0 )
	{
		if( ObjectTemp->PixelCount >= Min && ObjectTemp->PixelCount <= Max )			
			return 1;
		else
			return 0;
	}
	else
	{
		int size = ObjectTemp->Width * ObjectTemp->Height;
		if( size >= Min && size <= Max )
			return 1;
		else
			return 0;

	}


}

Object* Labeling::MakeNewObject( int x, int y )
{
	Object* NewObject = new Object;
	NewObject->Start.x = x;
	NewObject->Start.y = y;
	NewObject->Width = 1;
	NewObject->Height = 1;
	NewObject->PixelCount = 1;

	return NewObject;
}

void Labeling::SetParameter( int width, int height, int mode, int max, int min, int object, int background )
{
	Width = width;
	Height = height;
	SetMode( mode );
	ObjectValue = object;
	BackgroundValue = background;
	CheckField = new Table[ Width * Height ];
	for( int i = 0 ; i < Width * Height ; i++ )
		CheckField[i].IsVisit = 0;

	if( min == -1 )
		Min = 0;
	else
		Min = min;

	if( max == -1 )
		Max = Width*Height;
	else
		Max = max;


}
void Labeling::SetMode( int mode )
{
	ObjectMode = 1&mode;
	ExploreMode = (2&mode)/2;
	FilterMode = (4&mode)/4;

}
// about List
void Labeling::DeleteLastNode()
{
	if( List == NULL )
		return;
	ListNode* NodeTemp;
	NodeTemp = List->Top;

	List->Top = NodeTemp->next;
	List->NodeCount -= 1;
	
	delete NodeTemp->Data;
	delete NodeTemp;

	if( List->NodeCount == 0 )
		DestroyList();
	
}

void Labeling::DestroyList()
{
	if( List == NULL )
		return;
	else
	{
		ListNode* NodeTemp;

		while( List->Top != NULL )
		{
			NodeTemp = List->Top->next;
			delete List->Top->Data;
			delete List->Top;
			List->Top = NodeTemp;
		}

		delete List;
		List = NULL;

	}
}
Object* Labeling::PopList()
{
	if( List == NULL )
		return NULL;

	Object* ObjectTemp;
	ListNode* NodeTemp;

	NodeTemp = List->Top;
	ObjectTemp = NodeTemp->Data;
	List->Top = List->Top->next;
	
	delete NodeTemp;
	List->NodeCount -= 1;

	if( List->NodeCount == 0 )
		DestroyList();

	return ObjectTemp;	
}
int Labeling::IsEmptyList()
{
	if( List == NULL )
		return 1;
	else
		return 0;
}
void Labeling::PushList( Object* object )
{
	if( List == NULL )
	{
		List = new ListHeader;
		List->NodeCount = 0;
		List->Top = NULL;
	}

	ListNode* NewNode = new ListNode;
	NewNode->Data = object;
	
	NewNode->next = List->Top;
	List->Top = NewNode;
	List->NodeCount += 1;

}

