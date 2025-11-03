# EdaCal (Tarea EDA T3)

## Compilación
g++ -std=c++11 -O2 -Iinclude -o edacal.exe src\tokenizer.cpp src\shunting_yard.cpp edacal.cpp

## Uso de Test
./edacal < tests/edacal_tests.in | grep -E "^(ans ->|Error:)" > salida.txt
diff -u tests/edacal_expected.txt salida.txt

## Uso (ejemplos)
.\edacal.exe --version

# Interactivo
.\edacal.exe
6+5
posfix
prefix
tree
exit

# Script (PowerShell)
@"
6+5
posfix
prefix
exit
"@ | .\edacal.exe

## Funcionalidades
- Tokenizador (números, identificadores, (), + - * / ^, funciones).
- Shunting Yard (infija→posfija), maneja +/− unarios.
- Árbol de expresión construido desde la posfija.
- Evaluación y reporte de `posfix`, `prefix`, `tree`, `exit`.
- `sqrt` y `^` implementados.

## Errores considerados
- Paréntesis desbalanceados
- División por cero
- `sqrt` de negativo

