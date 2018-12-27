R"(
#version 330 core
out vec4 fragColor;

struct Light {
	vec3 position;
	vec3 direction;
	vec3 color;
	float intensity;
	float radius;
	float spotCutoff;
	int type; // 0 = DISABLED, 1 = SUN, 2 = POINT, 3 = SPOT
};

in DATA {
	vec4 color;
	vec4 position;
	vec3 normal;
	vec3 tangent;
	vec3 eye;
	vec2 uv;
	mat3 tbn;
} FSIn;

uniform mat4 uView;

uniform sampler2D uColor;

uniform sampler2D uNormal;
uniform bool uHasNormal;

uniform sampler2D uSpecular;
uniform bool uHasSpecular;

uniform sampler2D uEnv;
uniform bool uHasEnv;

uniform vec3 uAmbient = vec3(0.0);
uniform Light uLights[32];
uniform int uLightCount;

float sqr1(float x) { return x * x; }

float rim(vec3 n, vec3 v, float start, float end, float coef) {
	float rim = smoothstep(start, end, 1.0 - dot(n, v));
	return clamp(rim, 0.0, 1.0) * coef;
}

float lightAttenuation(Light light, vec3 L, float dist) {
	return sqr1(clamp(1.0 - sqr1(dist) / sqr1(light.radius), 0.0, 1.0));
}

vec3 gamma(vec3 col) {
	return pow(col, vec3(1.0 / 2.2));
}

vec3 calcLighting(vec3 N, float shininess, float specIntens) {
	vec3 lighting = vec3(0.0);
	vec3 P = FSIn.position.xyz;
	vec3 V = FSIn.eye;
	for (int i = 0; i < uLightCount; i++) {
		Light light = uLights[i];
		if (light.type == 0) {
			continue;
		}

		vec3 L = vec3(0.0);
		float att = 0.0;
		vec3 Lp = light.position;

		if (light.type == 1) { // SUN
			L = -light.direction;
			att = 1.0;
		} else if (light.type == 2) { // POINT
			L = Lp - P;
			float dist = length(L); L = normalize(L);

			if (dist < light.radius) {
				att = lightAttenuation(light, L, dist);
			} else { att = 0.0; }
		} else if (light.type == 3) { // SPOT
			L = Lp - P;
			float dist = length(L);
			L = normalize(L);

			att = lightAttenuation(light, L, dist);
			float S = dot(L, normalize(-light.direction));
			float c = cos(light.spotCutoff);
			if (S > c) {
				att *= (1.0 - (1.0 - S) * 1.0 / (1.0 - c));
			} else {
				att = 0.0;
			}
		}

		float NoL = dot(N, L);
		if (att > 0.0) {
			float nl = clamp(NoL, 0.0, 1.0);
			float fact = att * nl;
			float spec = 0.0;
			if (NoL > 0.0) {
				vec3 H = normalize(L + V);
				spec = pow(dot(N, H), 5.0 + shininess * 250.0) * specIntens;
			}
			vec3 specular = (spec * light.color);

			lighting += ((light.color * light.intensity) + specular) * fact;
		}
	}
	return (uAmbient + lighting);
}

void main() {
	vec3 N = FSIn.normal;
	vec4 S = vec4(vec3(0.0), 1.0);
	if (uHasNormal) {
		vec3 Nt = texture(uNormal, FSIn.uv).xyz * 2.0 - 1.0;
		N = normalize(FSIn.tbn * Nt);
	}

	if (uHasSpecular) {
		S = texture(uSpecular, FSIn.uv);
	}
	
	float shininess = clamp(S.r, 0.0, 1.0);
	float specIntens = S.g;

	vec4 color = texture(uColor, FSIn.uv);
	color.rgb *= mix(vec3(1.0), FSIn.color.rgb, S.b);
	if (color.a < 0.6) {
		discard;
	}

	vec3 env = vec3(0.0);
	if (uHasEnv) {
		float f0 = rim(N, FSIn.eye, 0.0, 0.6, 1.0);
		vec2 uv = vec2(uView * vec4(N, 0.0)) * 0.5 + 0.5;
		uv.y = 1.0 - uv.y;
		env = gamma(texture(uEnv, uv, (1.0 - shininess) * 3.0).rgb) * shininess * f0;
	}

	vec3 lighting = calcLighting(N, shininess, specIntens) * S.a;
	fragColor = vec4(lighting * (color.rgb + env), color.a);
}
)"