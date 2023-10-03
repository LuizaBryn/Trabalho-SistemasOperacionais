# Trabalho-SistemasOperacionais
Descrição do Problema
O objetivo deste trabalho é que o estudante aplique os conceitos de threads, exclusão mútua e coordenação de processos por meio do projeto e implementação de um jogo, conforme segue.

Um helicóptero, guiado pelo jogador, deve levar 10 soldados, da esquerda da tela para uma plataforma localizada à direita. Para impedir que o helicóptero cumpra a missão, há duas baterias antiaéreas autopropulsadas inimigas (bateria 0 e 1) que ficam continuamente disparando foguetes a esmo visando atingi-lo.  

O jogo termina numa das 3 situações abaixo:

o helicóptero de resgate cumprir a missão (vitória do jogador);
o helicóptero é abatido (derrota do jogador);
o helicóptero se chica contra algum obstáculo  (derrota do jogador).
Quando os foguetes terminam, a bateria deve se deslocar para a esquerda, onde há um depósito onde ele é recarregada. O tempo de recarga demora de 1/10 s a 1/2 s. Enquanto uma bateria está sendo recarregada, se a também necessita ser recarregada, ela deve aguardar.

Para uma bateria chegar ao depósito, há uma ponte no qual ela deve atravessar. Portanto, se a bateria 0 estiver atravessando a ponte, tanto no trajeto de ida quanto de volta, a bateria 1 deve aguardar e vice-versa

O jogador pode movimentar o helicóptero para frente (seta →), para trás (seta ←), para cima (seta ↑) e para baixo (seta ↓). Se o usuário não pressiona nenhuma tecla, o  helicóptero para. Se o helicóptero alcança o topo da tela (linha 0), ele explode. Se ele bate em algum obstáculo no solo (o próprio solo, alguma das baterias, o depósito ou a plataforma), ele também explode.

No anexo há uma versão tosca da interface usando apenas caracteres ASCII e compilável em ambiente Windows. Para alterar para Linux, a função void gotoxy(int x, int y) deve ser redefinida bem como o comando   system("cls"). O grupo pode usar essa interface ou uma melhorada (de preferência).

a capacidade de fabricação de foguetes é infinita, mas a bateria só consegue armazenar n foguetes;
o jogo tem 3 níveis de dificuldade:
fácil: as baterias têm poucos foguetes e o tempo de recarga é alto;
médio: as baterias têm um quantidade média de foguetes e o tempo de recarga é médio;
difícil: têm muitos foguetes e o tempo de recarga é baixo;
O jogo TEM que ser implementado em C ou C++ e usando pthreads.
