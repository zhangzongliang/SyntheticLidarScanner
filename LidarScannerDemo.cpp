/*=========================================================================
 *
 *  Copyright David Doria 2011 daviddoria@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

// Custom
#include "vtkLidarScanner.h"
#include "vtkLidarPoint.h"

// VTK
#include <vtkCubeSource.h>
#include <vtkImageData.h>
#include <vtkMath.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTriangleFilter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLImageDataWriter.h>

int main(int argc, char* argv[])
{
  bool createMesh = true;

  // Convert strings to doubles
  double tx = 0;
  double ty = -2;
  double tz = 0;
  double rx = 0;
  double ry = 0;
  double rz = 0;

  unsigned int thetaPoints = 2;
  unsigned int phiPoints = 3;

  double thetaSpan = .3;
  double phiSpan = .3;

  bool storeRays = true;

  // Sphere source
//   vtkSmartPointer<vtkSphereSource> inputSource =
//     vtkSmartPointer<vtkSphereSource>::New();
//   inputSource->Update();

  // Cube source
  vtkSmartPointer<vtkCubeSource> inputSource =
    vtkSmartPointer<vtkCubeSource>::New();
  inputSource->Update();

  // Cube source with conversion to triangles.
//   vtkSmartPointer<vtkCubeSource> cubeSource =
//     vtkSmartPointer<vtkCubeSource>::New();
//   cubeSource->Update();
// 
//   vtkSmartPointer<vtkTriangleFilter> inputSource = vtkSmartPointer<vtkTriangleFilter>::New();
//   inputSource->SetInputConnection(cubeSource->GetOutputPort());
//   inputSource->Update();

  {
  vtkSmartPointer<vtkXMLPolyDataWriter> writer = 
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName("ExampleInput.vtp");
  writer->SetInputConnection(inputSource->GetOutputPort());
  writer->Write();
  }

  // Construct a vtkLidarScanner and set all of its parameters
  vtkSmartPointer<vtkLidarScanner> scanner = 
    vtkSmartPointer<vtkLidarScanner>::New();

  //Scanner->WriteScanner("scanner_original.vtp");

  //double testAngle = vtkMath::Pi()/4.0;
  scanner->SetPhiSpan(phiSpan);
  scanner->SetThetaSpan(thetaSpan);

  scanner->SetNumberOfThetaPoints(thetaPoints);
  scanner->SetNumberOfPhiPoints(phiPoints);

  scanner->SetStoreRays(storeRays);

  // "Aim" the scanner.  This is a very simple translation, but any transformation will work
  vtkSmartPointer<vtkTransform> transform = 
    vtkSmartPointer<vtkTransform>::New();
  transform->PostMultiply();
  transform->RotateX(rx);
  transform->RotateY(ry);
  transform->RotateZ(rz);
  transform->Translate(tx, ty, tz);

  scanner->SetTransform(transform);
  scanner->WriteScanner("scanner_transformed.vtp");

  scanner->SetCreateMesh(createMesh);

  scanner->SetInputConnection(inputSource->GetOutputPort());
  scanner->Update();

  std::cout << "Before writer " << scanner->GetOutput()->GetNumberOfPoints() << " Points." << std::endl;

  int* dims = scanner->GetOutput()->GetDimensions();

  for (int y=0; y<dims[1]; y++)
    {
    for (int x=0; x<dims[0]; x++)
      {
      unsigned char* pixel = static_cast<unsigned char*>(scanner->GetOutput()->GetScalarPointer(x,y,0));
      std::cout << (int)pixel[0] << " " << (int)pixel[1] << " " << (int)pixel[2] << std::endl;
      }
    std::cout << std::endl;
    }

  // Write the output vti file
  vtkSmartPointer<vtkXMLImageDataWriter> vtiWriter =
    vtkSmartPointer<vtkXMLImageDataWriter>::New();
  vtiWriter->SetFileName("Scan.vti");
  vtiWriter->SetInputConnection(scanner->GetOutputPort());
  vtiWriter->Write();

  // Write the output vtp file
  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
  scanner->GetAllOutputPoints(polyData);
  std::cout << "There are " << polyData->GetNumberOfPoints() << " output points." << std::endl;
  vtkSmartPointer<vtkXMLPolyDataWriter> vtpWriter =
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  vtpWriter->SetFileName("Scan.vtp");
  vtpWriter->SetInputConnection(polyData->GetProducerPort());
  vtpWriter->Write();

  return EXIT_SUCCESS;
}
