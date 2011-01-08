#include "base.h"
#include "physics.h"
#include <d3dx9.h>
#include "engine.h"
#include "camera.h"

#define TIME_STEP 1.0f/60.0f
#define ITERATIONS 10

class CPhysicsDebugDraw : public b2DebugDraw
{
public:
    ~CPhysicsDebugDraw()
    {
        if (m_pLine != NULL)
        {
            m_pLine->Release();
            m_pLine = NULL;
        }
    }

    void Init()
    {
        D3DXCreateLine(Engine()->GetDevice(), &m_pLine);
        Engine()->AddObject(m_pLine);
    }

    // Draw a closed polygon provided in CCW order.
    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        if (m_pLine == NULL)
        {
            Init();
        }

        float scale = CPhysics::GetScale();

        int count = vertexCount+1;
        D3DXVECTOR3 *dxVertices = new D3DXVECTOR3[count];

        for (int32 i = 0; i < count; i++ )
        {
            int j = ( i == vertexCount ) ? 0:i;

            dxVertices[i] = D3DXVECTOR3( vertices[j].x, vertices[j].y, 0 ) * scale;
        }

        m_pLine->Begin();
        m_pLine->DrawTransform(dxVertices, count, &g_pCamera->GetProjection(), D3DCOLOR_ARGB(255, (int)(color.r*255), (int)(color.g*255), (int)(color.b*255) ) );
        m_pLine->End();

        delete dxVertices;
    }

    // Draw a solid closed polygon provided in CCW order.
    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {

    }

    // Draw a circle.
    void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
    {
        if (m_pLine == NULL)
        {
            Init();
        }

        #define CIRCLE_SEGMENTS 20

        D3DXVECTOR3 *dxVertices = new D3DXVECTOR3[CIRCLE_SEGMENTS+1];

        for (int i = 0; i <= CIRCLE_SEGMENTS; i++)
        {
            float angle = 2*D3DX_PI*(float(i)/(float)CIRCLE_SEGMENTS);

            dxVertices[i] = D3DXVECTOR3( center.x, center.y, 0.0f ) * CPhysics::GetScale() + (D3DXVECTOR3( cos(angle), sin(angle), 0.0f ) * radius * CPhysics::GetScale() );
        }

        m_pLine->Begin();
        m_pLine->DrawTransform(dxVertices, CIRCLE_SEGMENTS+1, &g_pCamera->GetProjection(), D3DCOLOR_ARGB(255, (int)(color.r*255), (int)(color.g*255), (int)(color.b*255) ) );
        m_pLine->End();

        delete dxVertices;
    }

    // Draw a solid circle.
    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
    {

    }

    // Draw a line segment.
    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
    {
        if (m_pLine == NULL)
        {
            Init();
        }

        float scale = CPhysics::GetScale();

        int count = 2;
        D3DXVECTOR3 *dxVertices = new D3DXVECTOR3[count];

        dxVertices[0] = D3DXVECTOR3( p1.x, p1.y, 0 ) * scale;
        dxVertices[1] = D3DXVECTOR3( p2.x, p2.y, 0 ) * scale;

        m_pLine->Begin();
        m_pLine->DrawTransform(dxVertices, count, &g_pCamera->GetProjection(), D3DCOLOR_ARGB(255, (int)(color.r*255), (int)(color.g*255), (int)(color.b*255) ) );
        m_pLine->End();

        delete dxVertices;
    }

    // Draw a transform. Choose your own length scale.
    // @param xf a transform.
    void DrawXForm(const b2XForm& xf)
    {
        int count = 2;
        D3DXVECTOR3 *dxVertices = new D3DXVECTOR3[count];

        b2Vec2 p1 = xf.position, p2;
        const float k_axisScale = 0.2f;
        
        float scale = CPhysics::GetScale();

        p2 = p1 + k_axisScale * xf.R.col1;

        dxVertices[0] = D3DXVECTOR3( p1.x, p1.y, 0 ) * scale;
        dxVertices[1] = D3DXVECTOR3( p2.x, p2.y, 0 ) * scale;        

        m_pLine->Begin();
        m_pLine->DrawTransform(dxVertices, count, &g_pCamera->GetProjection(), D3DCOLOR_ARGB(255,255,0,0) );
        m_pLine->End();

        p2 = p1 + k_axisScale * -xf.R.col2;

        dxVertices[0] = D3DXVECTOR3( p1.x, p1.y, 0 ) * scale;
        dxVertices[1] = D3DXVECTOR3( p2.x, p2.y, 0 ) * scale;        

        m_pLine->Begin();
        m_pLine->DrawTransform(dxVertices, count, &g_pCamera->GetProjection(), D3DCOLOR_ARGB(255,0,255,0) );
        m_pLine->End();

        delete dxVertices;
    }

private:
    ID3DXLine *m_pLine;
};

CPhysicsDebugDraw g_DebugDraw;

uint32 flagsDraw = CPhysicsDebugDraw::e_shapeBit|CPhysicsDebugDraw::e_aabbBit|CPhysicsDebugDraw::e_jointBit|CPhysicsDebugDraw::e_coreShapeBit|CPhysicsDebugDraw::e_centerOfMassBit;


bool CPhysics::Init()
{
    b2AABB world_aabb;
    world_aabb.lowerBound = -b2Vec2( GetScale(256.0f) , GetScale(256.0f) );
    world_aabb.upperBound =  b2Vec2( GetScale(256.0f) , GetScale(256.0f) );
    m_pWorld = new b2World( world_aabb, b2Vec2(0.0f, 1.0f), true );

    m_pWorld->SetDebugDraw(&g_DebugDraw);

    b2BodyDef wall_def;

    wall_def.fixedRotation = true;
    wall_def.position = b2Vec2(0.0f, 0.0f);
    
    m_pWalls = m_pWorld->CreateBody(&wall_def);
    
    b2PolygonDef groundShapeDef;
    groundShapeDef.restitution = 1.0f;
    
    // Left wall
    groundShapeDef.SetAsBox(0.1f, GetScale(260.0f), b2Vec2( 0.05f-GetScale(256.0f), 0.0f), 0.0f);
    m_pWalls->CreateShape(&groundShapeDef);
        
    // Right wall
    groundShapeDef.SetAsBox(0.1f, GetScale(260.0f), b2Vec2( GetScale(256.0f)-0.05f, 0.0f), 0.0f);
    m_pWalls->CreateShape(&groundShapeDef);

    // Bottom wall
    groundShapeDef.SetAsBox(GetScale(260.0f), 0.1f, b2Vec2( 0.0f, GetScale(256.0f)-0.05f), 0.0f);
    m_pWalls->CreateShape(&groundShapeDef);

    // Top wall
    groundShapeDef.SetAsBox( GetScale(260.0f), 0.1f, b2Vec2( 0.0f, 0.05f-GetScale(256.0f)), 0.0f);
    m_pWalls->CreateShape(&groundShapeDef);

    b2BodyDef ball_def;
    ball_def.position = b2Vec2(0.0f, -GetScale(200.0f) );
    
    m_pBall = m_pWorld->CreateBody(&ball_def);

    b2CircleDef ball_shape;
    ball_shape.radius = GetScale(32.0f);
    ball_shape.density = 1.0f;

    m_pBall->CreateShape(&ball_shape);
    m_pBall->SetMassFromShapes();

    return true;
}

void CPhysics::Destroy()
{
    if (m_pWorld != NULL)
    {
        delete m_pWorld;
        m_pWorld = NULL;
    }
}

void CPhysics::Update()
{
    if (m_pWorld)
    {
        m_pWorld->Step(TIME_STEP, ITERATIONS);
    }
}

void CPhysics::Render()
{
    if (m_pWorld)
    {
        // Set the flags needed to enable drawing
        g_DebugDraw.SetFlags(flagsDraw);

        // Draw the stopped world
        m_pWorld->Step(0, 0);

        // Clean the flags to disable draw
        g_DebugDraw.SetFlags(0);
    }
}