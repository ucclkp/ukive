// sigma = 1, [3]
static const float kr3[2] = { 0.2740686191, 0.4518627619 };
// sigma = 1, [5]
static const float kr5[3] = { 0.0544886845, 0.2442013420, 0.4026199469 };
// sigma = 1, [7]
static const float kr7[4] = { 0.0044330482, 0.0540055826, 0.2420362294, 0.3990502797 };
// sigma = 1, [9][11(-2)][13(-4)]
static const float kr9_13[5] = { 0.0001338306, 0.0044318616, 0.0539911274, 0.2419714457, 0.3989434694 };
// sigma = 10, [13]
static const float kr13[7] = { 0.0736662567, 0.0752660539, 0.0766007867, 0.0776552550, 0.0784173220, 0.0788781473, 0.0790323567 };


float4 gaussian_blur_3h(Texture2D tex, int x, int y)
{
    float4 color = tex.Load(int3(x, y, 0)) * kr3[1];
    for (int i = 0; i < 1; ++i)
    {
        color += tex.Load(int3(x - (1 - i), y, 0)) * kr3[i];
        color += tex.Load(int3(x + (1 - i), y, 0)) * kr3[i];
    }
    return color;
}

float4 gaussian_blur_3v(Texture2D tex, int x, int y)
{
    float4 color = tex.Load(int3(x, y, 0)) * kr3[1];
    for (int i = 0; i < 1; ++i)
    {
        color += tex.Load(int3(x, y - (1 - i), 0)) * kr3[i];
        color += tex.Load(int3(x, y + (1 - i), 0)) * kr3[i];
    }
    return color;
}

float4 gaussian_blur_5h(Texture2D tex, int x, int y)
{
    float4 color = tex.Load(int3(x, y, 0)) * kr5[2];
    for (int i = 0; i < 2; ++i)
    {
        color += tex.Load(int3(x - (2 - i), y, 0)) * kr5[i];
        color += tex.Load(int3(x + (2 - i), y, 0)) * kr5[i];
    }
    return color;
}

float4 gaussian_blur_5v(Texture2D tex, int x, int y)
{
    float4 color = tex.Load(int3(x, y, 0)) * kr5[2];
    for (int i = 0; i < 2; ++i)
    {
        color += tex.Load(int3(x, y - (2 - i), 0)) * kr5[i];
        color += tex.Load(int3(x, y + (2 - i), 0)) * kr5[i];
    }
    return color;
}

float4 gaussian_blur_7h(Texture2D tex, int x, int y)
{
    float4 color = tex.Load(int3(x, y, 0)) * kr7[3];
    for (int i = 0; i < 3; ++i)
    {
        color += tex.Load(int3(x - (3 - i), y, 0)) * kr7[i];
        color += tex.Load(int3(x + (3 - i), y, 0)) * kr7[i];
    }
    return color;
}

float4 gaussian_blur_7v(Texture2D tex, int x, int y)
{
    float4 color = tex.Load(int3(x, y, 0)) * kr7[3];
    for (int i = 0; i < 3; ++i)
    {
        color += tex.Load(int3(x, y - (3 - i), 0)) * kr7[i];
        color += tex.Load(int3(x, y + (3 - i), 0)) * kr7[i];
    }
    return color;
}

float4 gaussian_blur_13h(Texture2D tex, int x, int y)
{
    float4 color = tex.Load(int3(x, y, 0)) * kr13[6];
    for (int i = 0; i < 6; ++i)
    {
        color += tex.Load(int3(x - (6 - i), y, 0)) * kr13[i];
        color += tex.Load(int3(x + (6 - i), y, 0)) * kr13[i];
    }
    return color;
}

float4 gaussian_blur_13v(Texture2D tex, int x, int y)
{
    float4 color = tex.Load(int3(x, y, 0)) * kr13[6];
    for (int i = 0; i < 6; ++i)
    {
        color += tex.Load(int3(x, y - (6 - i), 0)) * kr13[i];
        color += tex.Load(int3(x, y + (6 - i), 0)) * kr13[i];
    }
    return color;
}

float4 gaussian_blur_9_13h(Texture2D tex, int x, int y)
{
    float4 color = tex.Load(int3(x, y, 0)) * kr9_13[4];
    for (int i = 0; i < 4; ++i)
    {
        color += tex.Load(int3(x - (4 - i), y, 0)) * kr9_13[i];
        color += tex.Load(int3(x + (4 - i), y, 0)) * kr9_13[i];
    }
    return color;
}

float4 gaussian_blur_9_13v(Texture2D tex, int x, int y)
{
    float4 color = tex.Load(int3(x, y, 0)) * kr9_13[4];
    for (int i = 0; i < 4; ++i)
    {
        color += tex.Load(int3(x, y - (4 - i), 0)) * kr9_13[i];
        color += tex.Load(int3(x, y + (4 - i), 0)) * kr9_13[i];
    }
    return color;
}