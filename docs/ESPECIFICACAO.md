# Especificação do produto — IBC Compressor de Imagem

## Objetivo

Reduzir imagens muito grandes para um limite escolhido pelo usuário, buscando manter a melhor aparência visual possível. O caso de referência é 200 MB para até 900 KB.

## Perfis planejados

| Perfil | Prioridade | Comportamento |
|---|---|---|
| Qualidade visual preservada | Equilíbrio | Mantém qualidade mínima conservadora e reduz dimensões quando necessário |
| Compressão máxima | Menor arquivo | Permite qualidade mais baixa antes de reduzir dimensões |
| Sem perdas | Integridade matemática | Não altera pixels; avisa quando o limite não puder ser alcançado |

O protótipo v0.1 contém os dois primeiros perfis. O perfil matematicamente sem perdas está especificado para a próxima versão porque requer regras próprias para transparência, profundidade de cor e compatibilidade de formato.

## Fluxo principal

1. O usuário arrasta uma ou várias imagens.
2. Escolhe o limite, por exemplo 900 KB.
3. Seleciona AVIF, WebP, JPEG ou PNG.
4. O motor aplica orientação, remove metadados se autorizado e codifica a imagem em memória.
5. Uma busca binária encontra a maior qualidade abaixo do limite.
6. Se necessário e permitido, a imagem é redimensionada progressivamente.
7. O arquivo final é gravado em uma pasta separada, sem substituir o original.

## Proteções

- nunca sobrescrever o arquivo original;
- não enviar imagens à internet;
- impedir mais de uma compressão simultânea;
- permitir cancelamento;
- informar falha de codec ou impossibilidade de atingir o alvo;
- preservar a orientação visual da fotografia;
- oferecer remoção de EXIF e outros metadados privados.

## Arquitetura

- **C++20:** leitura, codificação, busca de qualidade, lote e acesso a disco;
- **Qt 6:** portabilidade, codecs e empacotamento;
- **Qt Quick/QML:** interface responsiva;
- **Shader GLSL:** movimento holográfico de fundo processado pela GPU;
- **QtConcurrent:** processamento paralelo sem bloquear a interface.

## Plataformas-alvo

- macOS Universal: Apple Silicon e Intel no mesmo aplicativo;
- Windows 10/11 x64;
- futura avaliação de Windows ARM64 e Android, após estabilização do desktop.
