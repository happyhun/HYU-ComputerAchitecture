# HYU-ComputerAchitecture
컴퓨터구조론 프로젝트로 어셈블러와 시뮬레이터를 C로 직접 만들어보았습니다.  
  
프로젝트2는 hazard가 없다고 가정한 파이프라인 시뮬레이터입니다.

## 구현 언어 및 도구
- C
- VS Code

## 목차
1. [Assembler and Simulator](https://github.com/happyhun/HYU-ComputerAchitecture/tree/main/project1)
2. [Pipelined Simulator](https://github.com/happyhun/HYU-ComputerAchitecture/tree/main/project2)

## 이슈
1. 프로젝트1 명세를 처음 보고 눈물이 났다. C문법도 다 까먹었는데 24쪽짜리 영어 명세가 날라왔기 때문이다.
2. 명세만 일주일동안 읽어본 것 같다. 그러니까 갑자기 감이 잡히기 시작했다.
3. 처음에는 Machine Code를 문자열로 슬라이싱해야하나 생각했지만, C의 비트 연산자를 이용하면 간단히 해결되는 문제였다.
4. 프로젝트2는 Hazard가 없다고 가정하니 비교적 쉽게 완성했다.
  
파이썬 개발 가이드에서는 일단 예외가 없다고 가정하고 프로그램을 만든 후 예외를 다루라고 한다.  
  
프로젝트2를 하면서 왜 그렇게 개발하라고 권장하는지 알게 되었다.  
만약 처음부터 Hazard를 다루려고 했으면 현재의 프로그램도 완성하지 못 했을 것이다.
