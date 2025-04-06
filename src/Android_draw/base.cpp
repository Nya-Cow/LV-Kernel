//
//  module_tools.c
//  Dolphins
//
//  Created by 天溪 on 2022/4/25.
//

#include "base.h"


#pragma mark - 坐标系转换



ImVec3 matrixToVector(FMatrix matrix) {
  return ImVec3(matrix[3][0], matrix[3][1], matrix[3][2]);
}

FMatrix MatrixMulti(FMatrix m1, FMatrix m2) {
  FMatrix matrix = FMatrix();
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      for (int k = 0; k < 4; k++) {
        matrix[i][j] += m1[i][k] * m2[k][j];
      }
    }
  }
  return matrix;
}


FMatrix TransformToMatrix(FTransform transform)
{
  FMatrix matrix;
  matrix.M[3][0] = transform.Translation.x;
  matrix.M[3][1] = transform.Translation.y;
  matrix.M[3][2] = transform.Translation.z;
  float x2 = transform.Rotation.x + transform.Rotation.x;
  float y2 = transform.Rotation.y + transform.Rotation.y;
  float z2 = transform.Rotation.z + transform.Rotation.z;
  float xx2 = transform.Rotation.x * x2;
  float yy2 = transform.Rotation.y * y2;
  float zz2 = transform.Rotation.z * z2;
  matrix.M[0][0] = (1 - (yy2 + zz2)) * transform.Scale3D.x;
  matrix.M[1][1] = (1 - (xx2 + zz2)) * transform.Scale3D.y;
  matrix.M[2][2] = (1 - (xx2 + yy2)) * transform.Scale3D.z;
  float yz2 = transform.Rotation.y * z2;
  float wx2 = transform.Rotation.w * x2;
  matrix.M[2][1] = (yz2 - wx2) * transform.Scale3D.z;
  matrix.M[1][2] = (yz2 + wx2) * transform.Scale3D.y;
  float xy2 = transform.Rotation.x * y2;
  float wz2 = transform.Rotation.w * z2;
  matrix.M[1][0] = (xy2 - wz2) * transform.Scale3D.y;
  matrix.M[0][1] = (xy2 + wz2) * transform.Scale3D.x;
  float xz2 = transform.Rotation.x * z2;
  float wy2 = transform.Rotation.w * y2;
  matrix.M[2][0] = (xz2 + wy2) * transform.Scale3D.z;
  matrix.M[0][2] = (xz2 - wy2) * transform.Scale3D.x;
  matrix.M[0][3] = 0;
  matrix.M[1][3] = 0;
  matrix.M[2][3] = 0;
  matrix.M[3][3] = 1;
  return matrix;
}

FMatrix rotatorToMatrix(FRotator rotation) {
  float radPitch = rotation.Pitch * ((float) M_PI / 180.0f);
  float radYaw = rotation.Yaw * ((float) M_PI / 180.0f);
  float radRoll = rotation.Roll * ((float) M_PI / 180.0f);

  float SP = sinf(radPitch);
  float CP = cosf(radPitch);
  float SY = sinf(radYaw);
  float CY = cosf(radYaw);
  float SR = sinf(radRoll);
  float CR = cosf(radRoll);

  FMatrix matrix;

  matrix[0][0] = (CP * CY);
  matrix[0][1] = (CP * SY);
  matrix[0][2] = (SP);
  matrix[0][3] = 0;

  matrix[1][0] = (SR * SP * CY - CR * SY);
  matrix[1][1] = (SR * SP * SY + CR * CY);
  matrix[1][2] = (-SR * CP);
  matrix[1][3] = 0;

  matrix[2][0] = (-(CR * SP * CY + SR * SY));
  matrix[2][1] = (CY * SR - CR * SP * SY);
  matrix[2][2] = (CR * CP);
  matrix[2][3] = 0;

  matrix[3][0] = 0;
  matrix[3][1] = 0;
  matrix[3][2] = 0;
  matrix[3][3] = 1;

  return matrix;
}

void GetAxes(FRotator Rotation, ImVec3 *X, ImVec3 *Y, ImVec3 *Z) {
  float radPitch = (Rotation.Pitch * float(M_PI) / 180.f);
  float radYaw = (Rotation.Yaw * float(M_PI) / 180.f);
  float radRoll = (Rotation.Roll * float(M_PI) / 180.f);
  float SP = sinf(radPitch);
  float CP = cosf(radPitch);
  float SY = sinf(radYaw);
  float CY = cosf(radYaw);
  float SR = sinf(radRoll);
  float cro = cosf(radRoll);
  X->x = CP * CY;
  X->y = CP * SY;
  X->z = SP;
  Y->x = SR * SP * CY - cro * SY;
  Y->y = SR * SP * SY + cro * CY;
  Y->z = -SR * CP;
  Z->x = -(cro * SP * CY + SR * SY);
  Z->y = CY * SR - cro * SP * SY;
  Z->z = cro * CP;
}

float GetDis2D(ImVec2 self, ImVec2 object) {
  float osx = self.x - object.x;
  float osy = self.y - object.y;
  return sqrt(osx * osx + osy * osy);
}

bool GetInsideFov(float ScreenWidth, float ScreenHeight, ImVec2 PlayerBone, float FovRadius) {
  ImVec2 Cenpoint;
  Cenpoint.x = PlayerBone.x - ScreenWidth;
  Cenpoint.y = PlayerBone.y - ScreenHeight;
  if (Cenpoint.x * Cenpoint.x + Cenpoint.y * Cenpoint.y <= FovRadius * FovRadius) return true;
  return false;
}

float GetCenterOffsetForVector(ImVec2 point, ImVec2 CanvasSize) {
  return sqrt(pow(point.x - CanvasSize.x, 2.0f) + pow(point.y - CanvasSize.y, 2.0f));
}

/*雷达旋转*/
float RotateAngle(ImVec3 selfCoord, ImVec3 targetCoord) {
  float osx = targetCoord.x - selfCoord.x;
  float osy = targetCoord.y - selfCoord.y;
  return (float) (atan2(osy, osx) * 180 / M_PI);
}

// 计算旋转坐标

ImVec2 RotateCoord(float angle, ImVec2 coord) {
  float s = sin(angle * M_PI / 180);
  float c = cos(angle * M_PI / 180);
  return {coord.x * c + coord.y * s, -coord.x * s + coord.y * c};
}

//修改
float change(float num) {
  if (num < 0) {
    return abs(num);
  } else if (num > 0) {
    return num - num * 2;
  }
  return num;
}







