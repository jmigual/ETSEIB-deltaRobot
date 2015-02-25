
 
Shader "Diffuse Outline" {
    Properties {
        _Color ("Main Color", Color) = (1,1,1,1)
        _OutlineColor ("Outline Color", Color) = (0.8,0.5,0,0.8)
        _Outline ("Outline width", Range (.002, 0.03)) = .003
        _MainTex ("Texture", 2D) = "white" { }
        _BumpMap ("Bumpmap", 2D) = "bump" {}
        //_ToonShade ("ToonShader Cubemap(RGB)", CUBE) = "" { Texgen CubeNormal }
    }
   
    CGINCLUDE
    #include "UnityCG.cginc"
   
    struct appdata {
        float4 vertex : POSITION;
        float3 normal : NORMAL;
    };
 
    struct v2f {
        float4 pos : POSITION;
        float4 color : COLOR;
    };
   
    uniform float _Outline;
    uniform float4 _OutlineColor;
   
    v2f vert(appdata v) {
        v2f o;
        o.pos = mul(UNITY_MATRIX_MVP, v.vertex);
 
        float3 norm   = mul ((float3x3)UNITY_MATRIX_IT_MV, v.normal);
        float2 offset = TransformViewToProjection(norm.xy);
 
        o.pos.xy += offset * o.pos.z * _Outline;
        o.color = _OutlineColor;
        return o;
    }
    ENDCG
 
 
 
    SubShader {
 
        Tags { "Queue"="Transparent"}
        //UsePass "Diffuse/BASE"
          Pass {
            Name "OUTLINE"
            Tags { "LightMode" = "Always" }
            Cull off
            ZWrite Off
            ColorMask RGB
            Blend SrcAlpha OneMinusSrcAlpha
 
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
           
            half4 frag(v2f i) :COLOR { return i.color; }
            ENDCG
 
       }
       
 
       
 
       
            Tags { "RenderType" = "Opaque" }
            CGPROGRAM
            #pragma surface surf Lambert finalcolor:mycolor
            struct Input {
              float2 uv_MainTex;
              float2 uv_BumpMap;
            };
            sampler2D _MainTex;
            sampler2D _BumpMap;
            void surf (Input IN, inout SurfaceOutput o) {
              o.Albedo = tex2D (_MainTex, IN.uv_MainTex).rgb;
              o.Normal = UnpackNormal (tex2D (_BumpMap, IN.uv_BumpMap));
            }
            fixed4 _Color;
            void mycolor (Input IN, SurfaceOutput o, inout fixed4 color) {
              color *= _Color;
            }
            ENDCG
   }
 
 
 
    Fallback "Diffuse"
}