import { IonButton, IonCol, IonContent, IonFooter, IonGrid, IonHeader, IonInput, IonItem, IonList, IonMenu, IonPage, IonRow, IonSplitPane, IonTitle, IonToolbar, IonAvatar, IonLabel } from '@ionic/react';
import ExploreContainer from '../components/ExploreContainer';
import AppAppBar from '../components/AppAppBar';

import './Chats.css';

import firebase from 'firebase/compat/app';
import 'firebase/firestore';
import 'firebase/auth';

import { useAuthState, useSignInWithGoogle } from 'react-firebase-hooks/auth';
import { useCollectionData } from 'react-firebase-hooks/firestore';
import { GoogleAuthProvider, getAuth, signInWithPopup } from "firebase/auth";
import { addDoc, collection, getFirestore, limit, orderBy, query, serverTimestamp } from 'firebase/firestore';
import { useRef, useState } from 'react';

firebase.initializeApp({
  apiKey: "AIzaSyCEehOj0G0N2kNwPiZcydqGuU2bVwMA8Eo",
  authDomain: "qubechat-c7d77.firebaseapp.com",
  projectId: "qubechat-c7d77",
  storageBucket: "qubechat-c7d77.appspot.com",
  messagingSenderId: "195600294469",
  appId: "1:195600294469:web:3943a1bece516b5a50792c",
  measurementId: "G-N0MJT80RP7"
})

export const auth = getAuth();
const firestore = getFirestore();

const Chats: React.FC = () => {
  const [user] = useAuthState(auth);
  const [activeChat, setActiveChat] = useState<string>('chat1');

  return (
    <IonPage>
      <IonHeader>
        <AppAppBar title='Chats' />
      </IonHeader>
      <IonContent>
        <SignOut />
        {user ? <ChatRoom activeChat={activeChat} setActiveChat={setActiveChat} /> : <SignIn />}
      </IonContent>
    </IonPage>
  );
};

function SignIn() {
  const signInWithGoogle = () => {
    const provider = new GoogleAuthProvider();
    signInWithPopup(auth, provider);
  };

  return (
    <>
      <button className="sign-in" onClick={signInWithGoogle}>Sign in with Google</button>
    </>
  );
}

function SignOut() {
  return auth.currentUser && (
    <button onClick={() => auth.signOut()}>Sign Out</button>
  )
}

interface ChatRoomProps {
  activeChat: string;
  setActiveChat: (chatId: string) => void;
}

function ChatRoom({ activeChat, setActiveChat }: ChatRoomProps) {
  const dummy = useRef<HTMLSpanElement>(null);
  const messagesRef = collection(firestore, 'messages');
  const q = query(messagesRef, orderBy('createdAt'), limit(25));

  const [messages] = useCollectionData(q, { idField: 'id' });
  const [formValue, setFormValue] = useState('');

  const sendMessage = async (e: React.FormEvent) => {
    if (formValue === '') { return; }
    e.preventDefault();

    const { uid, photoURL } = auth.currentUser;

    await addDoc(messagesRef, {
      text: formValue,
      createdAt: serverTimestamp(),
      uid,
      photoURL
    })

    setFormValue('');
    if (dummy.current) {
      dummy.current.scrollIntoView({ behavior: 'smooth' });
    }
  }

  const contacts = [
    { id: 'chat1', name: 'Alice Johnson', avatar: 'https://i.pravatar.cc/150?img=1' },
    { id: 'chat2', name: 'Bob Smith', avatar: 'https://i.pravatar.cc/150?img=2' },
    { id: 'chat3', name: 'Charlie Brown', avatar: 'https://i.pravatar.cc/150?img=3' },
    { id: 'chat4', name: 'Diana Prince', avatar: 'https://i.pravatar.cc/150?img=4' },
    { id: 'chat5', name: 'Ethan Hunt', avatar: 'https://i.pravatar.cc/150?img=5' },
    { id: 'chat6', name: 'Fiona Gallagher', avatar: 'https://i.pravatar.cc/150?img=6' },
    { id: 'chat7', name: 'George Clooney', avatar: 'https://i.pravatar.cc/150?img=7' },
    { id: 'chat8', name: 'Hannah Montana', avatar: 'https://i.pravatar.cc/150?img=8' },
    { id: 'chat9', name: 'Ian Somerhalder', avatar: 'https://i.pravatar.cc/150?img=9' },
    { id: 'chat10', name: 'Jessica Alba', avatar: 'https://i.pravatar.cc/150?img=10' }
  ];

  return (
    <IonSplitPane when='xs' contentId='main'>
      <IonMenu contentId='main'>
        <IonHeader>
          <IonToolbar color='tertiary'>
            <IonTitle>Contacts</IonTitle>
          </IonToolbar>
        </IonHeader>
        <IonContent>
          <IonList>
            {contacts.map(contact => (
              <IonItem key={contact.id} button onClick={() => setActiveChat(contact.id)}>
                <IonAvatar slot="start">
                  <img src={contact.avatar} alt={`Avatar of ${contact.name}`} />
                </IonAvatar>
                <IonLabel>{contact.name}</IonLabel>
              </IonItem>
            ))}
          </IonList>
        </IonContent>
      </IonMenu>
      <div className='ion-page' id='main'>
        <IonHeader>
          <IonToolbar>
            <IonTitle>
              {contacts.find(contact => contact.id === activeChat)?.name}
            </IonTitle>
          </IonToolbar>
        </IonHeader>
        <IonContent className='ion-padding'>
          <div className='Chat'>
            {messages && messages.map(msg => <ChatMessage key={msg.id} message={msg} />)}
            <span ref={dummy}></span>
          </div>
        </IonContent>
        <IonFooter>
          <IonToolbar>
            <IonGrid>
              <IonRow>
                <IonCol>
                  <IonInput aria-label="text" onIonInput={(e) => setFormValue((e.target as HTMLInputElement).value)} placeholder='Enter text' value={formValue}></IonInput>
                </IonCol>
                <IonCol size='auto' className='ion-text-end'>
                  <IonButton fill='outline' onClick={sendMessage}>Send</IonButton>
                </IonCol>
              </IonRow>
            </IonGrid>
          </IonToolbar>
        </IonFooter>
      </div>
    </IonSplitPane>
  )
}

function ChatMessage(props: { message: { text: any; uid: any; photoURL: any; }; }) {
  const { text, uid, photoURL } = props.message;
  const messageClass = uid === auth.currentUser?.uid ? 'sent' : 'received';

  return (
    <>
      <div className={`message ${messageClass}`}>
        <img src={"https://ionicframework.com/docs/img/demos/avatar.svg"} alt="Avatar" />
        <p>{text}</p>
      </div>
    </>
  )
}

export default Chats;
