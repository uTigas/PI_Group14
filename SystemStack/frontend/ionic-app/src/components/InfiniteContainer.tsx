import { IonAvatar, IonContent, IonInfiniteScroll, IonInfiniteScrollContent, IonItem, IonLabel, IonList } from '@ionic/react';
import './InfiniteContainer.css';
import { useEffect, useState } from 'react';

interface ContainerProps {
  items: Array<string>;
  numItems: number;
}

const InfiniteContainer: React.FC<ContainerProps> = ({ items, numItems }) => {
  const [displayedItems, setDisplayedItems] = useState<string[]>(items.slice(0, numItems));
  const [hasMoreItems, setHasMoreItems] = useState<boolean>(true);

  const generateItems = () => {
    const startIndex = displayedItems.length;
    const endIndex = Math.min(startIndex + numItems, items.length);

    if (endIndex >= items.length) {
      setHasMoreItems(false); // Disable infinite scrolling if all items have been displayed
    }

    const newItems = items.slice(startIndex, endIndex);
    setDisplayedItems([...displayedItems, ...newItems]);
  };
  
  useEffect(() => {
    generateItems();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);

  return (
    <IonContent style={{height: "20vh"}} >
      <IonList>
        {displayedItems.map((item, index) => (
          <IonItem key={index}>
            <IonAvatar slot="start">
              <img src={'https://picsum.photos/80/80?random=' + index} alt="avatar" />
            </IonAvatar>
            <IonLabel>{item}</IonLabel>
            <IonLabel>{"Element at Index " + index}</IonLabel>
          </IonItem>
        ))}
      </IonList>
      <IonInfiniteScroll disabled={!hasMoreItems} onIonInfinite={(ev) => { generateItems(); setTimeout(() => ev.target.complete(), 500) }}>
        <IonInfiniteScrollContent ></IonInfiniteScrollContent>
      </IonInfiniteScroll>
    </IonContent>
  );
};

export default InfiniteContainer;
