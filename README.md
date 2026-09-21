# 1CCPI-FIAP-C-CS3

### Membros:

- Felipe de Oliveira Doern - RM 568798
- Miguel Marcelo Alves Ramos de Oliveira - RM 569467
- Tom Stringasci Albuquerque Coelho Morais - RM 568844
- Eric dos Santos Mendes da Silva - RM 569528
- Ligia de Andrade Matheus - RM 568973

## Funcionalidade:

Aprimorado da ultima vez, esse sistema serve como um protótipo de recarga complexo e dinâmico. Contas criadas dentro de uma execução são salvas no ``users.json`` para próximas execuções, junto com todas informações adicionadas dentro dessas contas. Seja carros, sua marcas, sua bateria, sua informação financeira de pix ou cartão de crédito, entre mais. 

Além disso, há agora um sistema individualizado de sessões. Antes, havia um log que salvava todas recargas, junto com o tipo de recarga feita, o horário, e quanto foi pago. Agora, ainda tem isso, e também tem as sessões que o usuário pode ver, para saber quando recarregou, por quanto, e o total de dinheiro gasto ou energia usada durante a existência de sua conta.

## Novidades em relação a funções ja existentes:

As funções ``carro()`` e ``recarga()`` foram mudadas completamente, separando sua lógica central do seu menu. Assim, não é preciso repetir o código para cada carro. Isso sozinho diminui o código por aproximadamente 700 linhas, e faz mudanças muito mais fáceis. Não só isso, mas o sistema completo de recarga foi mudado, com tempo de recarga mais dinâmico, e uma bateria extra que funciona mais realisticamente. Se o usuário não quiser usar a bateria durante uma recarga rápida, também é dado essa opção agora.

A função ``interface()`` também foi mudada para agora ter o tempo simulado da recarga, junto a carga da bateria extra.

## Análise Big O de algoritmos

Durante a implementação das sessões, foram usados 3 algoritmos diferentes, e os três tem um O(n) diferente.

### Busca Binaria: 
Foi usada para buscar as sessões. Sua classificação é de O(log n). O lado ruim, é que quando as sessões estão sorteadas por algo além de por ID, tem chance de falhar.

A busca binaria tem essa classificação pois faz uso do fato que a lista esta ordenada, e sempre compara a sua posição atual com a posição que quer chegar. Em uma lista de dez elementos, se quero o terceiro, ele vai cortar a lista ao meio, e checar se 3 é menor ou maior que 5. Se for menor, ele corta tudo acima de 5, e vai entre 1 e 5, repitindo até achar o elemento que quero. Mesmo em uma lista de 1 milhão de elementos, não demoraria mais que 20 passos.

### Busca Linear: 
Quando Busca Binaria falhar, o sistema implementa a Busca Linear que não pode falhar. Sua classificação é de O(n). O lado ruim é que é mais lento que a Busca Binaria, porém sempre funciona.

A busca linear tem essa classificação pois essencialmente varre a lista inteira, um por um, então o maximo de etapas que demorara é o numero de elementos em uma lista, ou seja, n.

### Bubble Sort: 
Usada para sortear as sessões de formas diferentes, as quatro opções sendo por ID, por custo, por tempo, e por energia consumida. Sua classificação é de O(n²). Demora muito mais que as outras duas, porém é necessaria para resortear todas sessões, enquanto as outras duas apenas servem para mostrar a informação da sessão.

A bubble sort flutua os maiores elementos para o topo da lista, comparando pares de elementos juntos e mudando sua posição se estiverem fora do lugar. Por necessitar laços aninhados de fors, sera n² pois é um for de n elementos vezes um for de n elementos. O maximo de etapas que demoraria para reordenar uma lista é se estivesse ao contrario.
