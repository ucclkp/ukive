struct PixelInputType {
    float4 position : SV_POSITION;
    float3 raw_position : POSITION;
};

float pow2(float val) {
    return val * val;
}

float distanceLinePoint(float2 pos, float2 sp, float2 ep) {
    float2 pv = pos - sp;
    float2 lv = ep - sp;
    float length_sq = lv.x * lv.x + lv.y * lv.y;
    float scale = dot(pv, lv) / length_sq;
    return length(pv - lv * scale);
}

float distanceOval(float2 pos, float rx, float ry) {
    float len = sqrt(pos.x * pos.x + pos.y * pos.y);
    float den = sqrt(pow2(rx * pos.y) + pow2(ry * pos.x));
    return abs(len * (1 - rx * ry / den));
}

float4 drawLine(float2 pos, float2 sp, float2 ep) {
    float4 color;
    float d = distanceLinePoint(pos, sp, ep);
    if (d < 0.5) {
        float2 t = float2(abs(ep.x - sp.x), abs(ep.y - sp.y));
        if (t.x > t.y) {
            float d1 = distanceLinePoint(float2(pos.x, pos.y + 1), sp, ep);
            float d2 = distanceLinePoint(float2(pos.x, pos.y - 1), sp, ep);
            if (d <= d1 && d <= d2) {
                color = float4(1, 0, 0, 1);
            } else {
                color = float4(1, 1, 1, 1);
            }
        } else {
            float d1 = distanceLinePoint(float2(pos.x + 1, pos.y), sp, ep);
            float d2 = distanceLinePoint(float2(pos.x - 1, pos.y), sp, ep);
            if (d <= d1 && d <= d2) {
                color = float4(1, 0, 0, 1);
            } else {
                color = float4(1, 1, 1, 1);
            }
        }
    } else {
        color = float4(1, 1, 1, 1);
    }

    return color;
}

float4 drawCircle(float2 pos, float2 center, float radius) {
    float2 opos = pos - center;

    float4 color;
    float r = abs(sqrt(opos.x * opos.x + opos.y * opos.y) - radius);
    if (r < 0.5) {
        float2 t = float2(abs(opos.y), abs(opos.x));
        if (t.x > t.y) {
            float ry1 = abs(sqrt(opos.x * opos.x + (opos.y + 1) * (opos.y + 1)) - radius);
            float ry2 = abs(sqrt(opos.x * opos.x + (opos.y - 1) * (opos.y - 1)) - radius);
            if (r <= ry1 && r <= ry2) {
                color = float4(1, 0, 0, 1);
            } else {
                color = float4(1, 1, 1, 1);
            }
        } else {
            float rx1 = abs(sqrt((opos.x + 1) * (opos.x + 1) + opos.y * opos.y) - radius);
            float rx2 = abs(sqrt((opos.x - 1) * (opos.x - 1) + opos.y * opos.y) - radius);
            if (r <= rx1 && r <= rx2) {
                color = float4(1, 0, 0, 1);
            } else {
                color = float4(1, 1, 1, 1);
            }
        }
    } else {
        color = float4(1, 1, 1, 1);
    }
    return color;
}

float4 drawOval(float2 pos, float2 center, float rx, float ry) {
    float2 opos = pos - center;
    float d = distanceOval(opos, rx, ry);

    float4 color;
    if (true) {
        float2 t = float2(abs(opos.y * (rx * rx)), abs(opos.x * (ry * ry)));
        if (t.x > t.y) {
            float d1 = distanceOval(float2(opos.x, opos.y + 1), rx, ry);
            float d2 = distanceOval(float2(opos.x, opos.y - 1), rx, ry);
            if (d <= d1 && d <= d2) {
                color = float4(1, 0, 0, 1);
            } else {
                color = float4(1, 1, 1, 1);
            }
        } else {
            float d1 = distanceOval(float2(opos.x + 1, opos.y), rx, ry);
            float d2 = distanceOval(float2(opos.x - 1, opos.y), rx, ry);
            if (d <= d1 && d <= d2) {
                color = float4(1, 0, 0, 1);
            } else {
                color = float4(1, 1, 1, 1);
            }
        }
    } else {
        color = float4(1, 1, 1, 1);
    }
    return color;
}

float4 fillCircle(float x, float y, float cx, float cy, float radius) {
    float4 color;
    float r = sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy));
    if (r > radius) {
        float diff = r - radius;
        if (diff < 1) {
            color = float4(1, 0, 0, 1 - diff);
        } else {
            color = float4(1, 1, 1, 1);
        }
    } else {
        color = float4(1, 0, 0, 1);
    }
    return color;
}

float4 main(PixelInputType input) : SV_TARGET{
    float x = input.raw_position.x;
    float y = input.raw_position.y;

    //return drawLine(input.raw_position.xy, float2(10, 10), float2(100, 50));
    //return drawLine(input.raw_position.xy, float2(10, 10), float2(100, 150));
    //return drawLine(input.raw_position.xy, float2(10, 50), float2(100, 10));
    //return drawLine(input.raw_position.xy, float2(10, 10), float2(100, 100));
    //return fillCircle(x, y, 100, 100, 5);
    //return drawCircle(input.raw_position.xy, float2(100.5, 100.5), 50);
    return drawOval(input.raw_position.xy, float2(100.5, 100.5), 50, 1);
}